#include <iostream>
#include <string>
#include <string.h>
#include <boost/tokenizer.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include "RpalParser.h"
#include "Fcns.h"
#include "Standardizer.h"
#include "CSElement.h"
#include "Flattener.h"
#include "CSEM.h"

using namespace std;

int main(int argc, char** argv)
{
  bool printAst, list, debug, tokens, build, stack, std_ast, print_cs;
  char* filename;

  printAst = false;
  list = false;
  debug = false;
  tokens = false;
  build = false;
  stack = false;
  std_ast = false;
  print_cs = false;

  for (int i = 1 ; i <= argc - 1; i++)
  {
    if (argv[i][0] != '-')
      filename = argv[i];
    else if (strcmp(argv[i], "-ast") == 0)
      printAst = true;
    else if (strcmp(argv[i], "-debug") == 0 || strcmp(argv[i], "-d") == 0)
      debug = true;
    else if (strcmp(argv[i], "-l") == 0)
      list = true;
    else if (strcmp(argv[i], "-tokens") == 0 || strcmp(argv[i], "-t") == 0)
      tokens = true;
    else if (strcmp(argv[i], "-stack") == 0 || strcmp(argv[i], "-s") == 0)
      stack = true;
    else if (strcmp(argv[i], "-build") == 0 || strcmp(argv[i], "-b") == 0)
      build = true;
    else if (strcmp(argv[i], "-standardize") == 0 || strcmp(argv[i], "-st") == 0)
      std_ast = true;
    else if (strcmp(argv[i], "-controlstructs") == 0 || strcmp(argv[i], "-cs") == 0)
      print_cs = true;
  } 

  RpalScanner sc(filename, list);
  string token;

  if (tokens)
  {
    while (sc.Has_Next())
    {
      token = sc.Next_Token();

      if (RpalScanner::IsID(token))
        token = "ID<" + token + ">";
      else if (RpalScanner::IsString(token))
        token = "STR<" + token + ">";
      else if (RpalScanner::IsInt(token))
        token = "INT<" + token + ">";
      else
        token = "'" + token + "'";

      cout << token << " \t ";
    } 
    cout << endl;
  }


  RpalParser parse(filename, debug, stack, build);
  Fcns* ast =  parse.GenerateAst();
  if (printAst)
  {
    ast->Print();
  }
  if (!printAst && debug)
  {
    cout << "\nParser debug run begin----------------------------------------\n\n";
    parse.GenerateAst();
  }

  Standardizer::StandardizeTree(ast);

  if (std_ast)
  {
      ast->Print();
  }

  CSL::cs_element cs = Flattener::Flatten(ast);

  if (print_cs)
  {
    Flattener::PrintCS(boost::get<CSL::cs_control_struct>(cs.detail));
  }

  //CSEM csem(cs);

  return 0;
}
