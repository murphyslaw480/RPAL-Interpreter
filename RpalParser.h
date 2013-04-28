#ifndef RPALSCANNER_H
#define RPALSCANNER_H
#include "RpalScanner.h"
#include "Fcns.h"
#include <stack>
#include <vector>
#include <string>

class RpalParser
{
  public:
    RpalParser(char* filename, bool debugMode, bool showStack, bool showBuild);
    Fcns* GenerateAst();
  private:
    stack<Fcns*> _trees;
    string _nt;  //next token
    RpalScanner *_scanner;
    bool _debugMode;
    bool _showStack;
    bool _showBuild;
    void build(string val, int n);  //build a tree with n children and a parent with value val
    void buildDebug(string val, int n);  //build a tree with n children and a parent with value val
    void read_token(string token);        //check that token matches, then consumes it
    void debug(string msg);
    void pushProc(string proc);   //for debugging recursion stack
    void popProc(string proc);   //for debugging recursion stack
    vector<string> _recursionStack;
    //parsing methods
    void E();
    void Ew();
    void T();
    void Ta();
    void Tc();
    void B();
    void Bt();
    void Bs();
    void Bp();
    void A();
    void At();
    void Af();
    void Ap();
    void R();
    void Rn();
    void D();
    void Da();
    void Dr();
    void Db();
    void Vb();
    void Vl();
};
#endif
