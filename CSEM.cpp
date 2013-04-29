#include "CSEM.h"
#include "Debug.h"
#include <iostream>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

using namespace std;
using namespace CSL;

CSEM::CSEM(cs_element top_cs)
{
    //push enclosing environment markers
    _control.push(CSL::make_env());
    _stack.push(CSL::make_env());
    //push all elements of top level control struct
    push_control_struct(boost::get<cs_control_struct>(top_cs.detail));
}

void CSEM::push_control_struct(cs_control_struct cs)
{
  for (int i = 0 ; i < cs.elements.size() ; i++)
  {
    _control.push(cs.elements[i]);
  }
}

//process top element on _control
void step();

//CSEM Rules
//rule 1
void stack_name(cs_element name_el)
{
  /*  ...Name     ...
   *  ...         Ob...     Where Ob=Lookup(Name, current_env)
   */
  ASSERT(name_el.type == r_id, "stack_name expected r_id, got " + name_el.type);
  cs_name name = boost::get<cs_name>(name_el.detail);
}
//rule 2
void stack_lambda(cs_element lam_el)
{
  /*  ...lam<x,k>     ...
   *  ...             lam<x,k,c>    Where c = current_env index
   */
  ASSERT(lam_el.type == r_id, "stack_lambda expected r_lambda, got " + lam_el.type);
  cs_lambda lambda = boost::get<cs_lambda>(lam_el.detail);
}
//rule 3 (6 and 7 with optimized machine)
void apply_op(cs_element op_el)
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
void apply_lambda_closure(cs_element lam_el)
{
  /*
   *  ...gamma      lam<x,k,c> Rand
   *  ...e<n>d<k>   e<n>
  */
  ASSERT(lam_el.type == r_lambda, "apply_lambda_closure expected r_lambda, got " + lam_el.type);
  cs_lambda lam = boost::get<cs_lambda>(lam_el.detail);
}
//rule 5
void exit_env(cs_element env_el)
{
  /*
   *  ...e<n>     value e<n>...
   *  ...         value...
  */
  ASSERT(env_el.type == r_env, "exit_env expected r_env, got " + env_el.type);
}
//rule 8
void apply_conditional(cs_element cond_el)
{
  /*
   *  ...<cond<d<then>d<else>>   true...
   *  ...
  */
  ASSERT(cond_el.type == r_cond, "apply_conditional expected r_cond, got " + cond_el.type);
  cs_cond cond = boost::get<cs_cond>(cond_el.detail);
}
//rule 9
void apply_tau(cs_element tau_el)
{
  /*
   *  ...t<n>     V_1 ... V_n ...
   *  ...         (V_1 ... V_n) ...
  */
  ASSERT(tau_el.type = r_tau, "apply_tau expected r_tau, got " + tau_el.type);
  cs_tau = boost::get<cs_tau>(tau_el.detail);
}
//rule 10
void apply_tuple_index(cs_element gam_el)
{
  /*
   *  ...gamma      (V_1 ... V_n) I ...
   *  ...           V_I           ...
  */
  ASSERT(gam_el.type = r_gamma, "apply_tuple_index expected r_gamma, got " + gam_el.type);
}

//open env(new_inx) on top of env(base_idx)
void open_env(int new_idx, int base_idx);
//lookup element in current environment
void lookup(cs_element name_el);

