#ifndef CSELEMENT_H
#define CSELEMENT_H

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

namespace CSL
{//Control Stact eLement

  //CSEM element types
  enum element_type
  {
    r_id,      //rpal identifier
    r_str,      //rpal string
    r_int,      //rpal int
    r_unop,       //rpal unary operator
    r_binop,       //rpal binary operator
    r_truth,      //rpal truth value
    r_tau,      //rpal tuple formation
    r_tuple,   //rpal tuple
    r_cs,      //control struct
    r_lambda,     //lambda expression
    r_cond,      //rpal -> (conditional operator)
    r_gamma,      //application
    r_env,   //environment marker
    r_nil,        //rpal nil
    r_dummy,      //rpal dummy
    r_ystar,      //ystar
    r_rec_lambda,      //recursive lambda
  };

  //forward definition
  struct cs_element;

  //a name (variable)
  struct cs_name
  {
    std::string name;
  };

  //An rpal string
  struct cs_str
  {
    std::string val;
  };

  //An rpal integer
  struct cs_int
  {
    int val;
  };

  //an rpal operator
  struct cs_op
  {
    std::string op;
  };

  //an rpal boolean
  struct cs_truth
  {
    bool val;
  };

  //an rpal tau (tuple formation)
  struct cs_tau
  {
    int n;  //number of elements to pop from stack and group together
  };

  //an rpal tuple
  struct cs_tuple
  {
    std::vector<cs_element> elements;
  };

  //a control struct (delta in notes)
  struct cs_control_struct
  {
    std::vector<cs_element> elements;
  };

  //a lambda
  struct cs_lambda
  {
    std::vector<cs_name> vars;
    int env;    //activation environment
    cs_control_struct control_struct;
  };

  //a conditional expression
  struct cs_cond
  {
    //if top of stack evaluates to true, expand clauses.first
    //else expand clauses.second
    std::pair<cs_control_struct, cs_control_struct> clauses;
  };

  //generic Control Stack element structure
  //anything that can be place on the control or stack is an instance of this
  struct cs_element
  {
    element_type type;
    //boost::variant<cs_control_struct, cs_lambda, cs_name, cs_str, cs_int, cs_op, cs_truth, cs_tuple> 
    boost::variant<cs_name, cs_str, cs_int, cs_op, cs_truth, cs_tau, cs_tuple, 
                   cs_control_struct, cs_lambda, cs_cond> detail;
  };

  std::ostream& operator<<(std::ostream& output, const cs_element& el);

  //easy cs_element construction

  cs_element make_int(int n);

  cs_element make_str(std::string s);

  cs_element make_truth(std::string truth_str);

  cs_element make_dummy();

  cs_element make_nil();

  cs_element make_name(std::string name);

  cs_element make_op(std::string op, element_type type);

  cs_element make_tau(int n);
   
  cs_element make_tuple(std::vector<cs_element> values);

  cs_element make_control_struct(std::vector<CSL::cs_element> element_list);

  cs_element make_lambda(std::vector<std::string> varnames, std::vector<CSL::cs_element> el_list, 
      int env);

  cs_element make_cond(std::vector<cs_element> if_true, std::vector<cs_element> if_false);
                       
  cs_element make_env();
}

#endif
