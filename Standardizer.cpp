#include "Standardizer.h"

void Standardizer::StandardizeTree(Fcns *top)
{
    if (top == NULL)
        return;

    if (top->firstChild != NULL)
        StandardizeTree(top->firstChild);

    if (top->nextSibling != NULL)
        StandardizeTree(top->nextSibling);

    if (top->value == "let")
    {
        standardizeLet(top);
    }

    else if (top->value == "within")
    {
        standardizeWithin(top);
    }

    else if (top->value == "function_form")
    {
        standardizeFcnForm(top);
    }

    else if (top->value == "lambda")
    {
        standardizeLambda(top);
    }

    else if (top->value == "and")
    {
        standardizeAnd(top);
    }

    else if (top->value == "@")
    {
        standardizeInfix(top);
    }

    else if (top->value == "where")
    {
        standardizeWhere(top);
    }

    else if (top->value == "rec")
    {
        standardizeRec(top);
    }
}

 void Standardizer::standardizeLet(Fcns *top)
{
    top->value = "gamma"; 
    Fcns *eq = top->firstChild;
    Fcns *p = eq->nextSibling;
    Fcns *x = eq->firstChild;
    Fcns *e = x->nextSibling;
    eq->value = "lambda";
    x->nextSibling = p;
    eq->nextSibling = e;
}

 void Standardizer::standardizeWithin(Fcns *top)
{
    top->value = "=";

    Fcns * eq1 = top->firstChild;
    Fcns * eq2 = eq1->nextSibling;

    Fcns * x1 = eq1->firstChild;
    Fcns * x2 = eq2->firstChild;
    eq1->firstChild = NULL;
    eq1->nextSibling = NULL;
    eq2->firstChild = NULL;

    Fcns * e1 = x1->nextSibling;
    Fcns * e2 = x2->nextSibling;
    x1->nextSibling = NULL;
    x2->nextSibling = NULL;

    top->firstChild = x2;
    x2->nextSibling = eq2;
    eq2->value = "gamma";
    eq2->firstChild = eq1;
    eq1->value = "lambda";
    eq1->nextSibling = e1;
    eq1->firstChild = x1;
    x1->nextSibling = e2;
}

 void Standardizer::standardizeFcnForm(Fcns *top)
{
    Fcns * p = top->firstChild;
    Fcns * v = p->nextSibling;
    top->value = "=";
    Fcns * lam = new Fcns("lambda", v);
    p->nextSibling = lam;
    Fcns * next = v->nextSibling;
    while (next->nextSibling != NULL)
    {
        lam = new Fcns("lambda");
        v->nextSibling = lam;
        lam->firstChild = next;
        v = next;
        next = v->nextSibling;
    }
}

 void Standardizer::standardizeLambda(Fcns *top)
{
    Fcns * v = top->firstChild;
    Fcns * next = v->nextSibling;
    while (next->nextSibling != NULL)
    {
        Fcns * lam = new Fcns("lambda", next);
        v->nextSibling = lam;
        v = next;
        next = next->nextSibling;
    }
}

 void Standardizer::standardizeAnd(Fcns *top)
{
    Fcns * comma = new Fcns(",");
    Fcns * tau = new Fcns("tau");
    comma->nextSibling = tau;
    Fcns * eq = top->firstChild;
    Fcns * x = eq->firstChild;
    Fcns * e = x->nextSibling;
    comma->firstChild = x;
    tau->firstChild = e;
    eq = eq->nextSibling;
    while (eq != NULL)
    {
        x->nextSibling = eq->firstChild;
        x = x->nextSibling;
        e->nextSibling = eq->firstChild->nextSibling;
        e = e->nextSibling;
        eq = eq->nextSibling;
    }
    x->nextSibling = NULL;
    top->value = "=";
    top->firstChild = comma;
}

 void Standardizer::standardizeInfix(Fcns *top)
{
    top->value = "gamma";
    Fcns * e1 = top->firstChild;
    Fcns * n = e1->nextSibling;
    Fcns * e2 = n->nextSibling;
    Fcns * gam = new Fcns("gamma", n);
    gam->nextSibling = e2;
    top->firstChild = gam;
    n->nextSibling = e1;
    e1->nextSibling = NULL;
}

 void Standardizer::standardizeWhere(Fcns *top)
{
    top->value = "gamma";
    Fcns * p = top->firstChild;
    Fcns * eq = p->nextSibling;
    Fcns * x = eq->firstChild;
    Fcns * e = x->nextSibling;
    eq->value = "lambda";
    top->firstChild = eq;
    x->nextSibling = p;
    p->nextSibling = NULL;
    eq->nextSibling = e;
}

 void Standardizer::standardizeRec(Fcns *top)
{
    top->value = "=";
    Fcns * x = top->firstChild->firstChild;
    delete top->firstChild;
    top->firstChild = new Fcns(x);
    Fcns * gam = new Fcns("gamma", new Fcns("<Y*>"));
    top->firstChild->nextSibling = gam;
    Fcns * lam = new Fcns("lambda", x);
    gam->firstChild->nextSibling = lam;
}
