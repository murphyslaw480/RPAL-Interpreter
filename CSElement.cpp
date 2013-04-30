#include "CSElement.h"
#include <cassert>
using namespace std;

namespace CSL
{ //Control Stack L-ement
  ostream& operator<<(ostream& output, const cs_element& el) 
  {
    switch (el.type)
    {
      case r_id:
        output << "<ID: " << boost::get<cs_name>(el.detail).name << ">";
        break;
      case r_int:
        output << "<INT: " << boost::get<cs_int>(el.detail).val << ">";
        break;
      case r_str:
        output << "<STR: " << boost::get<cs_str>(el.detail).val << ">";
        break;
      case r_truth:
        output << "<" << (boost::get<cs_truth>(el.detail).val ? "true" : "false") << ">";
        break;
      case r_nil:
        output << "<nil>";
        break;
      case r_dummy:
        output << "<dummy>";
        break;
      case r_env:
        output << "<e>";
        break;
      case r_cs:
        output << "<d>";
        break;
      case r_tau:
        output << "<tau>";
        break;
      case r_cond:
        output << "<cond>";
        break;
      case r_ystar:
        output << "<Y*>";
        break;
      case r_gamma:
        output << "<gamma>";
        break;
      case r_unop:
      case r_binop:
        output << "<OPERATOR: " << boost::get<cs_op>(el.detail).op << ">";
        break;
      case r_lambda:
        output << "<lambda(" ;
        int i;
        for (i = 0 ; i < boost::get<cs_lambda>(el.detail).vars.size() - 1 ; i++ )
        {
          output << (boost::get<cs_lambda>(el.detail).vars[i]).name << ", ";
        }
        output << (boost::get<cs_lambda>(el.detail).vars[i]).name;
        output << ") env= " << boost::get<cs_lambda>(el.detail).env.n << ">";
        break;
      case r_tuple:
        output << "(";
        for (i = 0 ; i < boost::get<cs_tuple>(el.detail).elements.size() - 1 ; i++)
        {
          output << boost::get<cs_tuple>(el.detail).elements[i] << ", ";
        }
        output << boost::get<cs_tuple>(el.detail).elements[i] << ")";
        break;
      default:
        output << "<UNKNOWN>";
    }
    return output;  // for multiple << operators.
  }

  cs_element make_int(int n)
  {
    cs_int detail;
    detail.val = n;
    cs_element el;
    el.type = r_int;
    el.detail = detail;
    return el;
  }

  cs_element make_str(string s)
  {
    cs_str detail;
    detail.val = s;
    cs_element el;
    el.type = r_str;
    el.detail = detail;
    return el;
  }

  cs_element make_truth(string truth_str)
  {
    cs_truth detail;
    detail.val = (truth_str.compare("true") == 0);
    cs_element el;
    el.type = r_truth;
    el.detail = detail;
    return el;
  }

  cs_element make_truth(bool truthval)
  {
    cs_truth detail;
    detail.val = truthval;
    cs_element el;
    el.type = r_truth;
    el.detail = detail;
    return el;
  }

  cs_element make_dummy()
  {
    cs_element el;
    el.type = r_dummy;
    return el;
  }

  cs_element make_nil()
  {
    cs_element el;
    el.type = r_nil;
    return el;
  }

  cs_element make_name(string name)
  {
    cs_element el;
    el.type = r_id;
    cs_name detail;
    detail.name = name;
    el.detail = detail;
    return el;
  }

  cs_element make_op(string op, element_type type)
  {
    assert(type == r_unop || type == r_binop);
    cs_element el;
    el.type = type;
    cs_op detail;
    detail.op = op;
    el.detail = detail;
    return el;
  }

  cs_element make_tau(int n)
  {
    cs_element el;
    el.type = r_tau;
    cs_tau detail;
    detail.n = n;
    el.detail = detail;
    return el;
  }

  cs_element make_tuple(vector<cs_element> values)
  {
    cs_element el;
    el.type = r_tuple;
    cs_tuple detail;
    detail.elements = values;
    el.detail = detail;
    return el;
  }

  cs_element make_control_struct(std::vector<CSL::cs_element> element_list)
  {
    cs_element el;
    el.type = r_cs;
    cs_control_struct detail;
    detail.elements = element_list;
    el.detail = detail;
    return el;
  }

  cs_element make_lambda(vector<string> varnames, vector<cs_element> el_list)
  {
    cs_element el;
    el.type = r_lambda;
    cs_lambda detail;
    for (int i = 0 ; i < varnames.size() ; i++)
    {
      cs_name name;
      name.name = varnames[i];
      detail.vars.push_back(name);
    }
    detail.control_struct.elements = el_list;
    el.detail = detail;
    return el;
  }

  cs_element make_lambda_with_env(cs_element lam_el, environment env)
  {
    cs_lambda lam = boost::get<cs_lambda>(lam_el.detail);
    lam.env = env;
    lam_el.detail = lam;
    return lam_el;
  }

  cs_element make_cond(vector<cs_element> if_true, vector<cs_element> if_false)
  {
    cs_element el;
    el.type = r_cond;
    cs_cond detail;
    cs_control_struct true_cs, false_cs;
    true_cs.elements = if_true;
    false_cs.elements = if_false;
    detail.clauses = std::make_pair<cs_control_struct,cs_control_struct>(true_cs, false_cs);
    el.detail = detail;
    return el;
  }

  cs_element make_env_marker(int idx)
  {
    cs_element el;
    el.type = r_env;
    return el;
  }

}
