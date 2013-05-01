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
        void Run();
        //process top element on _control
        void Step();
        void PrintControl();
        void PrintStack();
        void PrintEnv();
        void PrintCond();

        bool Done;

    private:

        std::stack<CSL::cs_element> _control;
        std::stack<CSL::cs_element> _stack;
        //stack of environments. _env_stack.top() gives active environment
        std::stack<CSL::environment> _env_stack;
        int _env_idx; //number of environments opened

        //expand a control struct onto _control
        void push_control_struct(CSL::cs_control_struct cs);

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
        //special functions
        void apply_function(CSL::cs_element fn_name_el);

        //recursion
        void apply_ystar();
        void apply_rec_lambda(CSL::cs_element rec_lam_el);

        //open env(new_inx) on top of env(base_idx)
        void open_env(int new_idx, int base_idx);
        //lookup element in current environment
        CSL::cs_element lookup(CSL::cs_element name_el);
};

#endif
