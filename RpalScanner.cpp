#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <ctype.h>
#include <boost/tokenizer.hpp>
#include "RpalScanner.h"

using namespace std;

const int RpalScanner::N_KEYWORDS = 20;
const string RpalScanner::KEYWORDS[] = 
  {"let", "in", "fn", "where", "aug", "or", "and", "not", "gr", "ls", "ge", "le", "eq", "ne", "rec", "within",
   "nil", "true", "false", "dummy"};

RpalScanner::RpalScanner(char* filename, bool showSource)
{
  string raw_source, line;
  ifstream input_file;
  input_file.open(filename);

  if (input_file.is_open())
  {
    while ( input_file.good() )
    {
      getline (input_file, line);
      //strip leading/trailing whitespace
      line.erase(remove(line.begin(), line.end(), '\t'), line.end());

      //strip comments
      int commentStart = line.find("//");
      if (commentStart != string::npos)
        line.erase(line.find("//"), string::npos);

      raw_source +=" " + line;   //add space so newlines are separated
    }
    input_file.close();

    if (showSource)
      cout << "Raw source: \n" << raw_source << endl << endl;

    boost::char_separator<char> sep("\n", " '()/*@&<>=+->|.,");
    Tokenizer tokenizer(raw_source, sep);
    process_tokens(tokenizer);
  }
  else
    cout << "File could not be opened\n";
}

void RpalScanner::process_tokens(Tokenizer tokenizer)
{
  Tokenizer::iterator iter = tokenizer.begin();
  while (iter != tokenizer.end())
  {
    string nt = "";
    //discard whitespace until a token or EOF is reached
    while (iter != tokenizer.end() && (nt == "" || nt == " " || nt == "\t"))
    {
      nt = *(iter++);
    }

    if (nt == "\'") //string, keep reading until ' closes string
    {
      while (*iter != "\'")
      {
        if(iter == tokenizer.end())
          cout << "Error: Encountered EOF without closing string";

        nt += *(iter++);
      }
      nt += *(iter++);
    }

    //special handling for multi-character operators
    else if (nt == "*" && *iter == "*")
      nt += *(iter++); //nt = **
    else if (nt == "-" && *iter == ">")
      nt += *(iter++); //nt = ->
    else if (nt == "<" && *iter == "=")
      nt += *(iter++); //nt = <=
    else if (nt == ">" && *iter == "=")
      nt += *(iter++); //nt = <=

    if (!(nt == " " || nt == "\t" || nt == ""))
      _tokens.push(nt);
  }
}

bool RpalScanner::Has_Next()
{
  return !(_tokens.empty());
}

string RpalScanner::Next_Token()
{
  string nt = _tokens.front();
  _tokens.pop();
  return nt;
}


  //token type identification methods (static)
  bool RpalScanner::IsID(string token)
  {
    if (!isalpha(token[0]))
      return false;
    for (int i = 0 ; i < N_KEYWORDS ; i++)
    {
      if (token == KEYWORDS[i])
        return false;

    }

    int i = 1;
    while (token[i])
    {
      if (!isalnum(token[i]) && token[i] != '_')
        return false;
      i++;
    }
    return true;
  }

  bool RpalScanner::IsString(string token)
  {
    return (token[0] == '\'' && *(token.rbegin()) == '\'');
  }

  bool RpalScanner::IsInt(string token)
  {
    if (!isdigit(token[0]))
      return false;

    int i = 1;

    while (token[i])
    {
      if (!isdigit(token[i]))
        return false;
      i++;
    }
    return true;
  }
