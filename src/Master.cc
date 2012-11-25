#include <iostream>
#include <vector>
#include "Nodes.h"
#include "Pathos.h"
using namespace std;

int main(int n, char* args[]) {
  Node* parsed = ParserFunctions::parse(ParserFunctions::load(args[1]));
  PathosUninterpreted* uninterpreted;
  if (parsed->isContainer()) {
    uninterpreted = new PathosUninterpretedList(dynamic_cast<ContainerNode*>(parsed), NativeFunctions::getStandardGlobals());
  }
  else {
    uninterpreted = new PathosUninterpretedText(dynamic_cast<TextNode*>(parsed)->getValue(), NativeFunctions::getStandardGlobals());
  }
  cout << uninterpreted->eval()->toString() << endl;;
}
