#define N_PATTERNS 13

#include "Flattener.h"
#include "Debug.h"
#include <utility>
#include <iostream>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

using namespace std;
using namespace CSL;

const pair<string,element_type> Flattener::element_pattern[] = 
{
  make_pair("^lambda$", r_lambda),                                             //lambda
  make_pair("^gamma$", r_gamma),                                               //gamma
  make_pair("^(not|neg)$", r_unop),                                            //rpal unary operator
  make_pair("^(aug|or|&|gr|ge|ls|le|eq|ne|\\+|-|\\*|/|\\*\\*)$", r_binop),     //rpal binary operator
  make_pair("^<(true|false)>$", r_truth),                                      //rpal truth value
  make_pair("^<nil>$", r_nil),                                                 //rpal nil
  make_pair("^<dummy>$", r_dummy),                                             //rpal dummy
  make_pair("^<STR:'(.*)'>$", r_str),                                          //rpal string
  make_pair("^<INT:\\d+>$", r_int),                                            //rpal int
  make_pair("^<ID:(\\w+)>$", r_id),                                            //rpal identifier
  make_pair("^tau$", r_tau),                                                   //rpal tuple formation
  make_pair("^->$", r_cond),                                                   //rpal -> (cond)
  make_pair("^<Y\\*>$", r_ystar)                                               //ystar
};

cs_element Flattener::Flatten(Fcns *st)
{
  vector<cs_element> element_list;
  flatten(st, element_list);
  return CSL::make_control_struct(element_list);
}

void Flattener::flatten(Fcns *node, vector<CSL::cs_element> &list)
{
    element_type et;
    bool match_found = false;
    for (int i = 0 ; i < N_PATTERNS ; i++)
    {
      //construct regex
      boost::regex e(element_pattern[i].first);
      boost::smatch what;
      if(boost::regex_match(node->value, what, e, boost::match_extra))
      {
        match_found = true;
        et = element_pattern[i].second;
      }
    }
    ASSERT(match_found, "Flattener could not match tree node: " + node->value);

    switch (et)
    {
      case r_lambda:
        {
          //get variable names
          vector<string> var_names;
          Fcns *var_node = node->firstChild;
          if (var_node->value == ",")
          { //multiple var function
            var_node = var_node->firstChild;
            while (var_node->nextSibling != NULL)
            {
              var_names.push_back(extract_id(var_node->value));
              var_node = var_node->nextSibling;
            }
          }
          else
          { //single var function
            var_names.push_back(extract_id(var_node->value));
          }
          //create control structure for lambda
          vector<cs_element> cs;
          flatten(node->firstChild->nextSibling, cs);
          //-1 for no env yet (not opened)
          list.push_back(CSL::make_lambda(var_names, cs, -1));
        }
        return; //dont flatten children onto current cs

      case r_id:
        {
          list.push_back(CSL::make_name(extract_id(node->value)));
        }
        break;

      case r_str:
        {
          list.push_back(CSL::make_str(extract_str(node->value)));
        }
        break;

      case r_int:
        {
          list.push_back(CSL::make_int(extract_int(node->value)));
        }
        break;

      case r_unop:
      case r_binop:
        {
          list.push_back(CSL::make_op(node->value, et));
        }
        break;

      case r_truth:
        {
          list.push_back(CSL::make_truth(node->value));
        }
        break;

      case r_tau:
        {
          Fcns *tup_el = node->firstChild;
          int n_tup_elements = 0;
          while (tup_el != NULL)
          {
            n_tup_elements++;
            tup_el = tup_el->nextSibling;
            ASSERT(n_tup_elements < 1000, "Tuple infinite loop limit reached. Tuple with over 1000 elements");
          }
          list.push_back(CSL::make_tau(n_tup_elements));
          tup_el = node->firstChild;
          while (tup_el != NULL)
          {
            flatten(tup_el, list);
            tup_el = tup_el->nextSibling;
          }
        }
        return; //children already flattened

      case r_cond:
        {
          vector<cs_element> cond_expression, true_clause, false_clause;
          //generate conditional expression control structure
          flatten(node->firstChild, cond_expression);
          //generate control struct to place on stact if cond_expression is true
          flatten(node->firstChild->nextSibling, true_clause);
          //generate control struct to place on stact if cond_expression is false
          flatten(node->firstChild->nextSibling->nextSibling, false_clause);
          list.push_back(CSL::make_cond(cond_expression, true_clause, false_clause));
        }
        return; //dont flatten children onto current list

      case r_ystar:
        {//recursion
          ASSERT(false, "Recursion not implemented");
        }
        break;

      case r_gamma:
      case r_nil:
      case r_dummy:
        { //gamma, nil, dummy have no special information
          cs_element generic_el;
          generic_el.type = et;
          list.push_back(generic_el);
        }
        break;

      default:
        {
          ASSERT(false, "Flattener could not flatten type " + et);
        }
        break;

    }

    //if not lambda or cond, should reach this point (lambda and cond return early)
    //flatten children into current control struct
    if (node->firstChild != NULL)
    {
      flatten(node->firstChild, list);
      if (node->firstChild->nextSibling != NULL)
      {
        flatten(node->firstChild->nextSibling, list);
      }
    }
}

string Flattener::extract_id(string node_val)
{
  boost::regex e("^<ID:(\\w+)>$");
  boost::smatch what;

  ASSERT(boost::regex_match(node_val, what, e, boost::match_extra),
      "could not extract identifier from " + node_val);

  string s(what[1].first, what[1].second);
  return s;
}

int Flattener::extract_int(string node_val)
{
  boost::regex e("^<INT:(\\d+)>$");
  boost::smatch what;

  ASSERT(boost::regex_match(node_val, what, e, boost::match_extra),
      "could not extract int from " + node_val);

  return boost::lexical_cast<int>(what[1]);
}

string Flattener::extract_str(string node_val)
{
  boost::regex e("^<STR:'(.*)'>$");
  boost::smatch what;

  ASSERT(boost::regex_match(node_val, what, e, boost::match_extra),
      "could not extract str from " + node_val);

  string s(what[1].first, what[1].second);
  return s;
}

void Flattener::PrintCS(CSL::cs_control_struct cs)
{
  cout << "CONTROL:\n";
  int cs_count = 0;
  print_cs(cs.elements, 0, cs_count);
}

void Flattener::print_cs(vector<cs_element> elements, int cs_num, int &cs_count)
{
  vector<pair<cs_control_struct,int> > sub_cs_list;

  cout << cs_num << ": ";
  for (int i = 0 ; i < elements.size() ; i++)
  {
    cout << " " << elements[i];
    if (elements[i].type == r_lambda)
    {
      cout << "(L" << ++cs_count << ")";
      sub_cs_list.push_back(
          make_pair(boost::get<cs_lambda>(elements[i].detail).control_struct, cs_count));
    }
    if (elements[i].type == r_cond)
    {
      cout << "(C" << ++cs_count <<"," << ++cs_count << "," << ++cs_count << ")";
      sub_cs_list.push_back(
          make_pair(boost::get<cs_cond>(elements[i].detail).truth_expression, cs_count-2));
      sub_cs_list.push_back(
          make_pair(boost::get<cs_cond>(elements[i].detail).clauses.first, cs_count-1));
      sub_cs_list.push_back(
          make_pair(boost::get<cs_cond>(elements[i].detail).clauses.second, cs_count));
    }
  }
  cout << " END\n\n";

  //print sub control structs
  for (int i = 0 ; i < sub_cs_list.size() ; i++)
  {
      print_cs(sub_cs_list[i].first.elements, sub_cs_list[i].second, cs_count);
  }
}
