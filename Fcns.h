#ifndef FCNS_H
#define FCNS_H
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

extern const int PrintDepthStart, PrintDepthIncrement;

//First-child, next sibling tree for construction of RPAL parser
class Fcns
{
    public:

    Fcns(string value);
    Fcns(string value, Fcns *child);
    Fcns(Fcns *copy);   //copy constructor

    void AddSibling(Fcns *sibling);

    void Print();
    string ToString();

    private:
    void preOrderPrint(int depth);
    string toString();
    
    string value;   //tree node identifier

    //first child is left branch, next sibling is right branch
    Fcns *firstChild, *nextSibling;

    friend class Standardizer;
    friend class Flattener;
    friend class CSEM;
};

#endif
