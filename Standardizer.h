#ifndef STANDARDIZER_H
#define STANDARDIZER_H
#include "Fcns.h"

class Standardizer
{
    public:
        static void StandardizeTree(Fcns *top);

    private:
        static void standardizeLet(Fcns *top);
        static void standardizeWithin(Fcns *top);
        static void standardizeFcnForm(Fcns *top);
        static void standardizeLambda(Fcns *top);
        static void standardizeAnd(Fcns *top);
        static void standardizeInfix(Fcns *top);
        static void standardizeWhere(Fcns *top);
        static void standardizeRec(Fcns *top);
};

#endif
