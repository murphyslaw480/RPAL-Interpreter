#include "libRPAL.h"
#include "Debug.h"
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <math.h>

using namespace std;
using namespace CSL;

namespace LIB_RPAL
{
  cs_element unop(cs_element rator, cs_element rand)
  {
    string op_str = boost::get<cs_op>(rator.detail).op;
    if (op_str.compare("not") == 0)
    {
      ASSERT(rand.type == r_truth, "neg expected int, got " + rand.type);
      return CSL::make_truth(!(boost::get<cs_truth>(rand.detail).val));
    }
    else if (op_str.compare("neg") == 0)
    {
      ASSERT(rand.type == r_int, "neg expected int, got " + rand.type);
      return CSL::make_int(-(boost::get<cs_int>(rand.detail).val));
    }
    else
    {
      ASSERT(false, "unknown unop " + op_str);
    }
  }

  cs_element binop(cs_element rator, cs_element rand1, cs_element rand2)
  {
    string op_str = boost::get<cs_op>(rator.detail).op;
    cs_element result;
    if (op_str.compare("aug") == 0)
    {
      if (rand1.type == r_tuple)
      {
        vector<cs_element> tup_vals = boost::get<cs_tuple>(rand1.detail).elements;
        tup_vals.push_back(rand2);
        return CSL::make_tuple(tup_vals);
      }
      else
      {
        vector<cs_element> tup_vals;
        //tup_vals.push_back(rand1);
        tup_vals.push_back(rand2);
        return CSL::make_tuple(tup_vals);
      }
    }
    else if (op_str.compare("&") == 0 || op_str.compare("or") == 0)
    {
      ASSERT(rand1.type == r_truth, "& expected truthval, got " + rand1.type);
      ASSERT(rand2.type == r_truth, "& expected truthval, got " + rand2.type);
      bool val1 = boost::get<cs_truth>(rand1.detail).val;
      bool val2 = boost::get<cs_truth>(rand2.detail).val;
      if (op_str.compare("&") == 0)
      {
        return CSL::make_truth(val1 && val2);
      }
      else
      {
        return CSL::make_truth(val1 || val2);
      }
    }
    else if (op_str.compare("ne") == 0 || op_str.compare("eq") == 0)
    { //multi-type comparison
      element_type t = rand1.type;
      ASSERT(t == rand2.type, ("Non-matching types for" + op_str));
      bool r;
      switch (t)
      {
        case r_int:
          r = (boost::get<cs_int>(rand1.detail).val == boost::get<cs_int>(rand2.detail).val);
          break;
        case r_str:
          r = (boost::get<cs_str>(rand1.detail).val.compare(boost::get<cs_str>(rand2.detail).val) == 0);
          break;
        case r_truth:
          r = (boost::get<cs_truth>(rand1.detail).val == boost::get<cs_truth>(rand2.detail).val);
          break;
        default:
          ASSERT(false, ("Invalid type for op " + op_str));
      }
      return CSL::make_truth(op_str.compare("eq") == 0 ? r : !r);
    }

    else
    { //anything else operates on ints
      ASSERT(rand1.type == r_int, ("numeric binop " + op_str + "expected int"));
      ASSERT(rand2.type == r_int, ("numeric binop " + op_str + "expected int"));
      int val1 = boost::get<cs_int>(rand1.detail).val;
      int val2 = boost::get<cs_int>(rand2.detail).val;
      if (op_str.compare("+") == 0)
      {
        return CSL::make_int(val1 + val2);
      }
      else if (op_str.compare("-") == 0)
      {
        return CSL::make_int(val1 - val2);
      }
      else if (op_str.compare("*") == 0)
      {
        return CSL::make_int(val1 * val2);
      }
      else if (op_str.compare("**") == 0)
      {
        return CSL::make_int(pow(val1 , val2));
      }
      else if (op_str.compare("/") == 0)
      {
        return CSL::make_int(val1 / val2);
      }
      else if (op_str.compare("gr") == 0)
      {
        return CSL::make_truth(val1 > val2);
      }
      else if (op_str.compare("ge") == 0)
      {
        return CSL::make_truth(val1 >= val2);
      }
      else if (op_str.compare("ls") == 0)
      {
        return CSL::make_truth(val1 < val2);
      }
      else if (op_str.compare("le") == 0)
      {
        return CSL::make_truth(val1 <= val2);
      }
      else
      {
        ASSERT(false, "unknown binop " + op_str);
      }
    }
  }
}
