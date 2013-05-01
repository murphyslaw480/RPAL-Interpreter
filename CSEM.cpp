#include "CSEM.h"
#include "Debug.h"
#include "libRPAL.h"
#include <iostream>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using namespace CSL;

CSEM::CSEM(cs_element top_cs)
{
    Done = false;
    //push enclosing environment markers
    _control.push(CSL::make_env_marker(0));
    _stack.push(CSL::make_env_marker(0));
    //push all elements of top level control struct
    push_control_struct(boost::get<cs_control_struct>(top_cs.detail));
    environment pe; //primitive environment
    pe.n = 0;
    _env_stack.push(pe);
    _env_idx = 0;
}

void CSEM::Run()
{
  while (!_control.empty())
  {
    Step();
  }
}

void CSEM::push_control_struct(cs_control_struct cs)
{
  for (int i = 0 ; i < cs.elements.size() ; i++)
  {
    _control.push(cs.elements[i]);
  }
}

//process top element on _control
void CSEM::Step()
{
  //pop top control element
  cs_element el = _control.top();
  _control.pop();
  switch(el.type)
  {
    case r_id:
      stack_name(el);
      break;
    case r_lambda:
      stack_lambda(el);
      break;
    case r_binop:
    case r_unop:
      apply_op(el);
      break;
    case r_gamma:
      if (_stack.top().type == r_lambda)
      {
        apply_lambda_closure(el);
      }
      else if (_stack.top().type == r_tuple)
      {
        apply_tuple_index(el);
      }
      else if (_stack.top().type == r_id)
      {
        //special function handling
        apply_function(_stack.top());
      }
      else if (_stack.top().type == r_ystar)
      {
        //special function handling
        apply_ystar();
      }
      else if (_stack.top().type == r_rec_lambda)
      {
        //special function handling
        apply_rec_lambda(_stack.top());
      }
      break;
    case r_env:
      exit_env(el);
      break;
    case r_cond:
      apply_conditional(el);
      break;
    case r_tau:
      apply_tau(el);
      break;
    case r_int:
    case r_truth:
    case r_str:
    case r_dummy:
    case r_nil:
    case r_ystar:
      _stack.push(el);
      break;
    default:
      cout << "Top of control: " << el;
      ASSERT(false, "CSEM could not step(). Element type on top of control:" + el.type);
  }
  Done = _control.empty();
}

//CSEM Rules
//rule 1
void CSEM::stack_name(cs_element name_el)
{
  /*  ...Name     ...
   *  ...         Ob...     Where Ob=Lookup(Name, current_env)
   */
  ASSERT(name_el.type == r_id, "stack_name expected r_id, got " + name_el.type);
  _stack.push(lookup(name_el));
}
//rule 2
void CSEM::stack_lambda(cs_element lam_el)
{
  /*  ...lam<x,k>     ...
   *  ...             lam<x,k,c>    Where c = current_env index
   */
  ASSERT(lam_el.type == r_lambda, "stack_lambda expected r_lambda, got " + lam_el.type);
  cs_lambda lambda = boost::get<cs_lambda>(lam_el.detail);
  _stack.push(CSL::make_lambda_with_env(lam_el, _env_stack.top()));
}
//rule 3 (6 and 7 with optimized machine)
void CSEM::apply_op(cs_element op_el)
{
  /* 
   * ...op   Rand {1,2}... 
   * ...     Result...
  */
  ASSERT(op_el.type == r_unop || op_el.type == r_binop, "apply_op expected r_op, got " + op_el.type);

  if (op_el.type == r_unop)
  {
    cs_element rand = _stack.top();
    _stack.pop();
    _stack.push(LIB_RPAL::unop(op_el, rand));
  }
  else
  {
    cs_element rand1 = _stack.top();
    _stack.pop();
    cs_element rand2 = _stack.top();
    _stack.pop();
    _stack.push(LIB_RPAL::binop(op_el, rand1, rand2));
  }
  
}
//rule 4  (and 11, for n-ary function)
void CSEM::apply_lambda_closure(cs_element gam_el)
{
  /*
   *  ...gamma      lam<x,k,c> Rand
   *  ...e<n>d<k>   e<n>
  */
  ASSERT(gam_el.type == r_gamma, "apply_lambda_closure expected r_gamma, got " + gam_el.type);
  cs_element lam_el = _stack.top();
  _stack.pop();   //pop lambda
  ASSERT(lam_el.type == r_lambda, ("apply_lambda_closure expects lambda on stack, got " + lam_el.type));
  //create new env
  vector<cs_name> names = boost::get<cs_lambda>(lam_el.detail).vars;
  environment closure_env = boost::get<cs_lambda>(lam_el.detail).env;
  closure_env.n = ++_env_idx;
  cs_control_struct cs = boost::get<cs_lambda>(lam_el.detail).control_struct;
  if (names.size() == 1)
  { //single var function
    ASSERT(!_stack.empty(), "Stack empty before lambda closure application finished");
    closure_env.substitutions.push_back(make_pair<string,cs_element>(names[0].name, _stack.top()));
    _stack.pop();
  }
  else
  {
    ASSERT(_stack.top().type == r_tuple, "n-ary function expected tuple on top of stack");
    vector<cs_element> rand = boost::get<cs_tuple>(_stack.top().detail).elements;
    _stack.pop();
    for (int i = 0 ; i < names.size() ; i++)
    {
      ASSERT(!_stack.empty(), "Stack empty before lambda closure application finished");
      closure_env.substitutions.push_back(make_pair<string,cs_element>(names[i].name, rand[i]));
    }
  }
  _env_stack.push(closure_env);
  _control.push(CSL::make_env_marker(_env_idx));
  push_control_struct(cs);
  _stack.push(CSL::make_env_marker(_env_idx));
}
//rule 5
void CSEM::exit_env(cs_element env_el)
{
  /*
   *  ...e<n>     value e<n>...
   *  ...         value...
  */
  ASSERT(env_el.type == r_env, "exit_env expected r_env, got " + env_el.type);
  ASSERT(boost::get<cs_env>(env_el.detail).n == _env_stack.top().n, "Control Env marker idx doesn't match open env");
  stack<cs_element> values;
  while (_stack.top().type != r_env)
  {
    ASSERT(!_stack.empty(), "Stack empty before env exit finished");
    values.push(_stack.top());
    _stack.pop();
  }
  ASSERT(boost::get<cs_env>(_stack.top().detail).n == _env_stack.top().n, "Stack Env marker idx doesn't match open env");
  _stack.pop();   //pop env close marker
  while (!values.empty())
  {
    _stack.push(values.top());
    values.pop();
  }
  _env_stack.pop();
}
//rule 8
void CSEM::apply_conditional(cs_element cond_el)
{
  /*
   *  ...<cond<d<then>d<else>>   true...
   *  ...
  */
  ASSERT(cond_el.type == r_cond, "apply_conditional expected r_cond, got " + cond_el.type);
  cs_cond cond = boost::get<cs_cond>(cond_el.detail);
  ASSERT(_stack.top().type == r_truth, "apply_conditional didn't find truth on stack");
  if (boost::get<cs_truth>(_stack.top().detail).val)
  { //true on stack
    push_control_struct(cond.clauses.first);
  }
  else
  {
    push_control_struct(cond.clauses.second);
  }
  //pop bool from stack
  _stack.pop();
}
//rule 9
void CSEM::apply_tau(cs_element tau_el)
{
  /*
   *  ...t<n>     V_1 ... V_n ...
   *  ...         (V_1 ... V_n) ...
  */
  ASSERT(tau_el.type == r_tau, "apply_tau expected r_tau, got " + tau_el.type);
  cs_tau tau = boost::get<cs_tau>(tau_el.detail);
  vector<cs_element> values;
  for (int i = 0 ; i < tau.n ; i++)
  {
    ASSERT(!_stack.empty(), "Stack empty before tau finished");
    values.push_back(_stack.top());
    _stack.pop();
  }
  _stack.push(CSL::make_tuple(values));
}
//rule 10
void CSEM::apply_tuple_index(cs_element gam_el)
{
  /*
   *  ...gamma      (V_1 ... V_n) I ...
   *  ...           V_I           ...
  */
  ASSERT(gam_el.type == r_gamma, "apply_tuple_index expected r_gamma, got " + gam_el.type);
  cs_element tup_el = _stack.top();
  _stack.pop();
  cs_element idx_el = _stack.top();
  _stack.pop();
  ASSERT(idx_el.type == r_int, "apply tuple index expected int index, got " + idx_el.type);
  _stack.push(boost::get<cs_tuple>(tup_el.detail).elements[boost::get<cs_int>(idx_el.detail).val - 1]);
}

//lookup element in current environment
cs_element CSEM::lookup(cs_element name_el)
{
  ASSERT(name_el.type == r_id, "lookup expected r_name, got " + name_el.type);
  string name = boost::get<cs_name>(name_el.detail).name;
  vector<pair<string,cs_element> > subs = _env_stack.top().substitutions;
  for (int i = 0 ; i < subs.size() ; i++)
  {
    if (name.compare(subs[i].first) == 0)
    {
      return subs[i].second;
    }
  }
  //no match found, return name
  return name_el;
}

void CSEM::apply_function(CSL::cs_element fn_name_el)
{
  string fn_name = boost::get<cs_name>(fn_name_el.detail).name;
  if (fn_name.compare("Print") == 0 || fn_name.compare("print") == 0)
  {
    _stack.pop();
    cout << _stack.top();
    _stack.pop();
  }
  else if (fn_name.compare("Conc") == 0 || fn_name.compare("conc") == 0)
  {
    _stack.pop();   //pop Conc
    ASSERT(_stack.top().type == r_str, "Cannot conc non-string");
    string s1 = boost::get<cs_str>(_stack.top().detail).val;
    _stack.pop();   //pop str1
    ASSERT(_stack.top().type == r_str, "Cannot conc non-string");
    string s2 = boost::get<cs_str>(_stack.top().detail).val;
    _stack.pop();   //pop str2
    _stack.push(CSL::make_str(s1 + s2));
  }
  else if (fn_name.compare("Order") == 0)
  {
    _stack.pop();   //pop Conc
    ASSERT(_stack.top().type == r_tuple, "Cannot compute order of non-tuple");
    int tup_size = boost::get<cs_tuple>(_stack.top().detail).elements.size();
    _stack.pop();   //pop tuple
    _stack.push(CSL::make_int(tup_size));
  }
  //type checking methods
  else if (fn_name.compare("Istuple") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_tuple));
  }
  else if (fn_name.compare("Isinteger") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_int));
  }
  else if (fn_name.compare("Isstring") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_str));
  }
  else if (fn_name.compare("Isdummy") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_dummy));
  }
  else if (fn_name.compare("Istruthvalue") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_truth));
  }
  else if (fn_name.compare("Isfunction") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_lambda || t == r_rec_lambda));
  }
  else if (fn_name.compare("Null") == 0)
  {
    _stack.pop();   //pop Istuple
    element_type t = _stack.top().type;
    _stack.pop();   //pop element to check
    _stack.push(CSL::make_truth(t == r_nil));
  }
  else if (fn_name.compare("Stem") == 0)
  {
    _stack.pop();   //pop Stem
    ASSERT(_stack.top().type == r_str, "Cannot compute Stem of non-string");
    string s = boost::get<cs_str>(_stack.top().detail).val;
    _stack.pop();   //pop element to compute Stem of
    _stack.push(CSL::make_str(s.substr(0,1)));
  }
  else if (fn_name.compare("Stern") == 0)
  {
    _stack.pop();   //pop Stern
    ASSERT(_stack.top().type == r_str, "Cannot compute Stern of non-string");
    string s = boost::get<cs_str>(_stack.top().detail).val;
    _stack.pop();   //pop element to compute Stem of
    _stack.push(CSL::make_str(s.substr(1,s.length()-1)));
  }
  else if (fn_name.compare("ItoS") == 0)
  { //int to string
    _stack.pop();   //pop ItoS
    ASSERT(_stack.top().type == r_str, "Cannot compute ItoS of non-int");
    int num = boost::get<cs_int>(_stack.top().detail).val;
    _stack.pop();   //pop int
    string s = boost::lexical_cast<string>(num);
    _stack.push(CSL::make_str(s));
  }
  else
  {
    ASSERT(false, ("Failed applying gamma to " + fn_name));
  }
}

void CSEM::PrintControl()
{
  cout << "C| ";
  stack<cs_element> temp;
  while (!_control.empty())
  {
    temp.push(_control.top());
    _control.pop();
  }
  while (!temp.empty())
  {
    cout << " " << temp.top() << " ";
    _control.push(temp.top());
    temp.pop();
  }
}

void CSEM::PrintStack()
{
  stack<cs_element> temp;
  while (!_stack.empty())
  {
    cout << " " <<_stack.top() << " ";
    temp.push(_stack.top());
    _stack.pop();
  }
  while (!temp.empty())
  {
    _stack.push(temp.top());
    temp.pop();
  }
  cout << " |S";
}

void CSEM::PrintEnv()
{
  cout << "ENV " << _env_stack.top().n << ":";
  for (int i = 0 ; i < _env_stack.top().substitutions.size() ; i++)
  {
    cout << _env_stack.top().substitutions[i].second << " / "
      << _env_stack.top().substitutions[i].first << " , ";
  }
  cout << "\n";
}

void CSEM::PrintCond()
{
  std::pair<cs_control_struct, cs_control_struct> clauses 
    = boost::get<cs_cond>(_control.top().detail).clauses;
  cout << "IF_TRUE: " << CSL::make_control_struct(clauses.first.elements, clauses.first.idx) << "\n";
  cout << "IF_FALSE: " << CSL::make_control_struct(clauses.second.elements, clauses.first.idx) << "\n";
}

void CSEM::apply_ystar()
{
  _stack.pop(); //pop y*
  _stack.top().type = r_rec_lambda;
}

void CSEM::apply_rec_lambda(CSL::cs_element rec_lam_el)
{
  _stack.push(_stack.top());
  _stack.top().type = r_lambda;
  cs_element gam;
  gam.type = r_gamma;
  _control.push(gam);
  _control.push(gam);
}
