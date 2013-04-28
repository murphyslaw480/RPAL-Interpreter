#ifndef FLATTENER_H
#define FLATTENER_H

#include "Fcns.h"
#include "CSElement.h"

#include <vector>
#include <string>
#include <utility>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

class Flattener
{
  public:
    static CSL::cs_element Flatten(Fcns *node);
    static void PrintCS(CSL::cs_control_struct cs);
  private:
    static void flatten(Fcns *node, std::vector<CSL::cs_element> &list);
    static const std::pair<std::string,CSL::element_type>  element_pattern[]; 

    //data extraction from tree node strings
    static std::string extract_id(string node_val);
    static int extract_int(string node_val);
    static std::string extract_str(string node_val);
    static void print_cs(std::vector<CSL::cs_element> elements, int cs_num, int &cs_count);
};

#endif
