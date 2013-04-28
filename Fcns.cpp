#include "Fcns.h"

using namespace std;

const int PrintDepthStart = 0;
const int PrintDepthIncrement = 1;

Fcns::Fcns(string s)
{
    value = s;
    firstChild = NULL;
    nextSibling = NULL;
}

Fcns::Fcns(string s, Fcns *child)
{
    value = s;
    firstChild = child;
    nextSibling = NULL;
}

Fcns::Fcns(Fcns *copy)
{
    value = copy->value;
    firstChild = (copy->firstChild == NULL) ?
                    NULL : new Fcns(copy->firstChild);
    nextSibling = (copy->nextSibling == NULL) ?
                    NULL : new Fcns(copy->nextSibling);
}

void Fcns::AddSibling(Fcns *sibling)
{
    if (nextSibling == NULL)
    {
        nextSibling = sibling;
    }
    else
    {
        nextSibling->AddSibling(sibling);
    }
}

void Fcns::Print()
{
    preOrderPrint(PrintDepthStart);
}

string Fcns::ToString()
{
  return toString();
}

string Fcns::toString()
{
  stringstream s;
  s << "<" << value << ">";
  if (nextSibling != NULL)
  {
    s << " -> " << nextSibling->toString();
  }
  return s.str();
}

void Fcns::preOrderPrint(int depth)
{
    for (int i = 0 ; i < depth ; i++)
    {
        cout << '.';
    }

    cout << value << "\n";

    if (firstChild != NULL)
    {
        firstChild->preOrderPrint(depth + PrintDepthIncrement);
    }

    if (nextSibling != NULL)
    {
        nextSibling->preOrderPrint(depth);
    }

}


