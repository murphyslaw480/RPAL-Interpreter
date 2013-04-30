#ifndef LIB_RPAL
#define LIB_RPAL
#include "CSElement.h"
#include <string>
namespace LIB_RPAL
{
  CSL::cs_element unop(CSL::cs_element rator, CSL::cs_element rand);
  CSL::cs_element binop(CSL::cs_element rator, CSL::cs_element rand1, CSL::cs_element rand2);
}
#endif
