#ifndef CSEM_H
#define CSEM_H

#include "CSElement.h"

#include <vector>
#include <stack>
#include <string>

//number of different element types that can be on control/stack
#define NUM_ELEMENTS 15

class CSEM
{
    public:
        CSEM(CSL::cs_element top_cs, bool debug);

    private:
        struct environment
        {
            int n;      //environment index
            //env upon which this env is based
            environment *base_env;  
            //(variable,value) substitutions
            std::vector<std::pair<CSL::cs_name,CSL::cs_element> > substitutions;
        };

        std::stack<CSL::cs_element> _control;
        std::stack<CSL::cs_element> _stack;
        //stack of active environments
        std::stack<environment> _env_stack;
        //list of all environments opened
        std::vector<environment> _env_list;

        void push_control_struct(CSL::cs_control_struct cs);

        bool _debug_mode;
        void debug(std::string msg);
};

#endif
