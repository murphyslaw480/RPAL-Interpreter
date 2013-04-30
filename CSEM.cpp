#include "CSEM.h"
#include "Debug.h"
#include "libRPAL.h"
#include <iostream>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

using namespace std;
using namespace CSL;

CSEM::CSEM(cs_element top_cs)
{
    //push enclosing environment markers
    _control.push(CSL::make_env_marker(0));
    _stack.push(CSL::make_env_marker(0));
    //push all elements of top level control struct
    push_control_struct(boost::get<cs_control_struct>(top_cs.detail));
    environment pe; //primitive environment
    _env_stack.push(pe);
    _env_idx = 0;
}

void CSEM::push_control_struct(cs_control_struct cs)
{
  for (int i = 0 ; i < cs.elements.size() ; i++)
  {
    _control.push(cs.elements[i]);
  }
}

//process top element on _control
void CSEM::step()
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
      else
      {
        //TODO: handle special functions
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
    default:
      ASSERT(false, "CSEM could not step(). Element type on top of control:" + el.type);
  }
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
  ASSERT(lam_el.type == r_id, "stack_lambda expected r_lambda, got " + lam_el.type);
  cs_lambda lambda = boost::get<cs_lambda>(lam_el.detail);
}
//rule 3 (6 and 7 with optimized machine)
void CSEM::apply_op(cs_element op_el)
{
  /* 
   * ...op   Rand {1,2}... 
   * ...     Result...
  */
  ASSERT(op_el.type == r_unop || op_el.type == r_binop, "apply_op expected r_op, got " + op_el.type);

  cs_op op = boost::get<cs_op>(op_el.detail);
  if (op_el.type == r_unop)
  {
  }
  else
  {
  }
  
}
//rule 4  (and 11, for n-ary function)
void CSEM::apply_lambda_closure(cs_element lam_el)
{
  /*
   *  ...gamma      lam<x,k,c> Rand
   *  ...e<n>d<k>   e<n>
  */
  ASSERT(lam_el.type == r_lambda, "apply_lambda_closure expected r_lambda, got " + lam_el.type);
  cs_lambda lam = boost::get<cs_lambda>(lam_el.detail);
}
//rule 5
void CSEM::exit_env(cs_element env_el)
{
  /*
   *  ...e<n>     value e<n>...
   *  ...         value...
  */
  ASSERT(env_el.type == r_env, "exit_env expected r_env, got " + env_el.type);
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
}
//rule 10
void CSEM::apply_tuple_index(cs_element gam_el)
{
  /*
   *  ...gamma      (V_1 ... V_n) I ...
   *  ...           V_I           ...
  */
  ASSERT(gam_el.type == r_gamma, "apply_tuple_index expected r_gamma, got " + gam_el.type);
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


