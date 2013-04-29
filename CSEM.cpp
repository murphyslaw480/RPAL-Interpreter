#include "CSEM.h"
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

void CSEM::debug(string msg)
{
  if (_debug_mode)
    cout << msg;
}

void CSEM::push_control_struct(cs_control_struct cs)
{
  for (int i = 0 ; i < cs.elements.size() ; i++)
  {
    _control.push(cs.elements[i]);
  }
}

