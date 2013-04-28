#ifndef RPALPARSER_H
#define RPALPARSER_H
#include "RpalParser.h"
#include <stack>
#include <string>
#include <sstream>
#include <iostream>

RpalParser::RpalParser(char* filename, bool debugMode, bool showStack, bool showBuild)
{
  _scanner = new RpalScanner(filename, false);
  _nt = _scanner->Next_Token();
  _debugMode = debugMode;
  _showStack = showStack;
  _showBuild = showBuild;
}

Fcns* RpalParser::GenerateAst()
{
  E();
  debug("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Primary E() complete<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
  return _trees.top();
}

void RpalParser::build(string val, int n)
{
  stringstream s;
  s << "\n---------------build(" << val << ", " << n << ")\tstack size: " << _trees.size() << "--------------------\n"; 
  debug(s.str());

  if (_showBuild)
  {
    buildDebug(val, n);
    return;
  }

  if (n == 0)
  {
    _trees.push(new Fcns(val));
  }
  else
  {
    Fcns *current = _trees.top();
    for (int i = 0 ; i < n-1 ; i++)
    {
      _trees.pop();
      _trees.top()->AddSibling(current);
      current = _trees.top();
    }
    _trees.pop();
    _trees.push(new Fcns(val, current));
  }

}

void RpalParser::buildDebug(string val, int n)
{

  if (n == 0)
  {
    _trees.push(new Fcns(val));
    debug("\tpushed: " + _trees.top()->ToString() + "\n"); 
  }
  else
  {
    Fcns *current = _trees.top();
    for (int i = 0 ; i < n-1 ; i++)
    {
      debug("\tpopping: " + _trees.top()->ToString() + "\n"); 
      _trees.pop();
      debug("\tadding: " + current->ToString() + " as sibling to " + _trees.top()->ToString() + "\n"); 
      _trees.top()->AddSibling(current);
      current = _trees.top();
      debug("\tnew current: " + current->ToString() + "\n"); 
    }
    debug("\tpopping: " + _trees.top()->ToString() + "\n");
    _trees.pop();
    _trees.push(new Fcns(val, current));
    debug("\tpushed: " + _trees.top()->ToString() + "\n"); 
    _trees.top()->Print();
    debug("\n");
  }
  stringstream s;
  s << "\n---------------new stack size: " << _trees.size() << "--------------------\n"; 
  debug(s.str());
}

void RpalParser::read_token(string token)
{
  debug("\nRead:\t'" + token + "'");

  if (token != _nt)
    cout << "\nError, RpalParser::read('" << token << "') recieved '" << _nt << "'" << endl;

  if (_scanner->Has_Next())
  {
    _nt = _scanner->Next_Token();
    debug("\t Next up: '" + _nt + "'\n");
  }
  else
  {
    _nt = "";
    debug("++++++++++++++++++++END OF INPUT++++++++++++++++++++\n");
  }

  if (RpalScanner::IsID(token))
    build("<ID:" + token + ">", 0);
  else if (RpalScanner::IsInt(token))
    build("<INT:" + token + ">", 0);
  else if (RpalScanner::IsString(token))
    build("<STR:" + token + ">", 0);
}

void RpalParser::debug(string msg)
{
  if (_debugMode)
    cout << msg;
}

void RpalParser::pushProc(string proc)
{
  if (!_showStack)
    return;

  debug("\n*** after pop " + proc + " Stack:");
  _recursionStack.push_back(proc);
  for (int i = 0 ; i < _recursionStack.size() ; i++)
    debug("  |  " + _recursionStack[i]);
  debug("\n");
}

void RpalParser::popProc(string proc)
{
  if (!_showStack)
    return;

  debug("\n*** after pop " + proc + " Stack:");
  _recursionStack.pop_back();
  for (int i = 0 ; i < _recursionStack.size() ; i++)
    debug("  |  " + _recursionStack[i]);
  debug("\n");
}

//parsing methods
void RpalParser::E()
{
  pushProc("E()");
  if (_nt == "let")
  {
      read_token("let");
      D();
      read_token("in");
      E();
      build("let", 2);
  }
  else if(_nt == "fn")
  {
      read_token("fn");
      Vb() ; int n = 1;
      //while (RpalScanner::IsID(_nt) || _nt == "(")
      while(_nt != ".")
      {
        Vb(); n++;
      }
      read_token(".");
      E();
      build("lambda", n+1);
  }
  else
  {
      Ew();
  }
  popProc("E()");
}

void RpalParser::Ew()
{
  pushProc("Ew()");
  T();
  if (_nt == "where")
  {
    read_token("where");
    Dr();
    build("where", 2);
  }
  popProc("Ew()");
}

void RpalParser::T()
{
  pushProc("T()");
  Ta();
  int n = 1;
  while (_nt == ",")
  {
    read_token(",");
    Ta();
    n++;
  }
  if (n > 1)
  {
    build("tau", n);
  }
  popProc("T()");
}

void RpalParser::Ta()
{
  pushProc("Ta()");
  Tc();
  while (_nt == "aug")
  {
    read_token("aug");
    Tc();
    build("aug", 2);
  }
  popProc("Ta()");
}

void RpalParser::Tc()
{
  pushProc("Tc()");
  B();
  if (_nt == "->")
  {
    read_token("->");
    Tc();
    read_token("|");
    Tc();
    build("->", 3);
  }
  popProc("Tc()");
}

void RpalParser::B()
{
  pushProc("B()");
  Bt();
  while (_nt == "or")
  {
    read_token("or");
    Bt();
    build("or", 2);
  }
  popProc("B()");
}

void RpalParser::Bt()
{
  pushProc("Bt()");
  Bs();
  while (_nt == "&")
  {
    read_token("&");
    Bs();
    build("&", 2);
  }
  popProc("Bt()");
}

void RpalParser::Bs()
{
  pushProc("Bs()");
  if (_nt == "not")
  {
    read_token("not");
    Bp();
    build("not", 1);
  }
  //else
  Bp();
  popProc("Bs()");
}

void RpalParser::Bp()
{
  pushProc("Bp()");
  A();
  if (_nt == "gr" || _nt == "ls" || _nt == "ge" || _nt == "le" || _nt == "eq" || _nt == "ne"
      || _nt == ">" || _nt == "<" || _nt == ">=" || _nt == "<=")
  {
    string temp = _nt;
    read_token(_nt);
    A();

    if (temp == "gr" || temp == ">")
      build("gr", 2);
    else if (temp == "ge" || temp == ">=")
      build("ge", 2);
    else if (temp == "ls" || temp == "<")
      build("ls", 2);
    else if (temp == "le" || temp == "<=")
      build("le", 2);
    else if (temp == "eq")
      build("eq", 2);
    else if (temp == "ne")
      build("ne", 2);
    else
      cout << "Error parsing conditional token " << temp << " before " << _nt;

  }
  popProc("Bp()");
}

void RpalParser::A()
{
  pushProc("A()");
  if (_nt == "+")
  {
    read_token("+");
    At();
  }
  else if (_nt == "-")
  {
    read_token("-");
    At();
    build("neg", 1);
  }
  else
  {
    At();
    /*
    string temp = _nt;
    while (_nt == "+" || _nt == "-")
    {
      read_token(_nt);
      At();
      build(temp, 2);
    }
    */
  }
  while (_nt == "+" || _nt == "-")
  {
    string temp = _nt;
    read_token(_nt);
    At();
    build(temp, 2);
  }
  popProc("A()");
}

void RpalParser::At()
{
  pushProc("At()");
  Af();
  string temp = _nt;
  while (_nt == "*" || _nt == "/")
  {
    read_token(_nt);
    Af();
    build(temp, 2);
    temp = _nt;
  }
  popProc("At()");
}

void RpalParser::Af()
{
  pushProc("Af()");
  Ap();
  while (_nt == "**")
  {
    read_token(_nt);
    Af();
    build("**", 2);
  }
  popProc("Af()");
}

void RpalParser::Ap()
{
  pushProc("Ap()");
  R();  //read preceding operand 
  while (_nt == "@")
  {
    read_token(_nt);  //read @
    if (!RpalScanner::IsID(_nt))
    {
      cout << "Error: Expected <ID> after token @" << endl;
      cout << "Instead recieved: " << _nt << endl;
    }
    read_token(_nt);  //read <id> following @
    R();    //read operand following @<id>
    build("@", 3);
  }
  popProc("Ap()");
}

void RpalParser::R()
{
  pushProc("R()");
  Rn();
  while (_nt == "true" || _nt == "false" || _nt == "nil" || _nt == "dummy" || _nt == "(" ||
      RpalScanner::IsInt(_nt) || RpalScanner::IsString(_nt) || RpalScanner::IsID(_nt))
  {
    Rn();
    build("gamma", 2);
  }

  popProc("R()");
}

void RpalParser::Rn()
{
  pushProc("Rn()");
  if (RpalScanner::IsInt(_nt) || RpalScanner::IsString(_nt) || RpalScanner::IsID(_nt))
  {
    read_token(_nt);
  }
  else if (_nt == "(")
  {
    read_token(_nt);
    E();
    read_token(")");
  }
  else if (_nt == "true" || _nt == "false" || _nt == "nil" || _nt == "dummy")
  {
    string temp = _nt;
    read_token(_nt);
    build("<" + temp + ">", 0);
  }
  popProc("Rn()");
}

void RpalParser::D()
{
  pushProc("D()");
  Da();
  if (_nt == "within")
  {
    read_token(_nt);
    D();
    build("within", 2);
  }
  popProc("D()");
}

void RpalParser::Da()
{
  pushProc("Da()");
  Dr();
  int n = 1;
  while (_nt == "and")
  {
    read_token(_nt);
    Dr();
    n++;
  }
  if (n > 1)
  {
    build("and", n);
  }
  popProc("Da()");
}

void RpalParser::Dr()
{
  pushProc("Dr()");
  if (_nt == "rec")
  {
    read_token(_nt);
    Db();
    build("rec", 1);
  }
  else
    Db();
  popProc("Dr()");
}

void RpalParser::Db()
{
  pushProc("Db()");
  if (_nt == "(")
  {
    read_token(_nt);
    D();
    read_token(")");
  }
  else
    if (!RpalScanner::IsID(_nt))
    {
      cout << "\nError: Expecting <ID> in process Db() but recieved token " << _nt << endl;
    }
    read_token(_nt);
    if (_nt == "," || _nt == "=")   //Vl '=' E
    {
      int n = 1;
      while (_nt == ",")
      {
        read_token(_nt);
        read_token(_nt);
        n++;
      }
      if (n > 1)
      {
        build (",", n);
      }
      read_token("=");
      E();
      build("=", 2);
    }
    else    //<ID> Vb+ '=' E
    {
      int n = 1;
      while (RpalScanner::IsID(_nt) || _nt == "(")
      {
        Vb();
        n++;
      }
      if (n <= 1)
        cout << "\nError: proc Db expected () or <ID> for fcn_form, but recieved " << _nt << endl;
      read_token("=");
      E();
      n++;
      build("function_form", n);
    }
    popProc("Db()");
}

void RpalParser::Vb()
{
  pushProc("Vb()");
  if (_nt == "(")
  {
    read_token(_nt);
    if (RpalScanner::IsID(_nt))
      Vl();
  }
  else
    read_token(_nt);
  popProc("Vb()");
}

void RpalParser::Vl()
{
  pushProc("Vl()");
  read_token(_nt);
  int n = 1;
  while (_nt == ",")
  {
    read_token(_nt);
    read_token(_nt);
    n++;
  }
  if (n > 1)
  {
    build(",", n);
  }
  read_token(")");
  popProc("Vl()");
}
#endif
