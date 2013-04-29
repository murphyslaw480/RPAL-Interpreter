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
        CSEM(CSL::cs_element top_cs);

    private:
        struct environment
        {
            int n;      //environment index
            //(variable,value) substitutions
            std::vector<std::pair<CSL::cs_name,CSL::cs_element> > substitutions;
        };

        std::stack<CSL::cs_element> _control;
        std::stack<CSL::cs_element> _stack;
        //stack of active environments
        std::stack<environment> _env_stack;
        //list of all environments opened
        std::vector<environment> _env_list;

        //expand a control struct onto _control
        void push_control_struct(CSL::cs_control_struct cs);
        //process top element on _control
        void step();

        //CSEM Rules
        //rule 1
        void stack_name(CSL::cs_element name_el);
        //rule 2
        void stack_lambda(CSL::cs_element lam_el);
        //rule 3 (6 and 7 with optimized machine)
        void apply_op(CSL::cs_element op_el);
        //rule 4  (and 11, for n-ary function)
        void apply_lambda_closure(CSL::cs_element lam_el);
        //rule 5
        void exit_env(CSL::cs_element env_el);
        //rule 8
        void apply_conditional(CSL::cs_element cond_el);
        //rule 9
        void apply_tau(CSL::cs_element tau_el);
        //rule 10
        void apply_tuple_index(CSL::cs_element gam_el);

        //open env(new_inx) on top of env(base_idx)
        void open_env(int new_idx, int base_idx);
        //lookup element in current environment
        void lookup(CSL::cs_element name_el);
};

#endif
