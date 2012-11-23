#include <iostream>
#include <vector>
#include "Nodes.h"
#include "Pathos.h"
using namespace std;

int main(int n, char* args[]) {
  unordered_map<string, PathosAtom*> globals;
  globals["+"] = new PathosNativeFunction(0);
  globals["-"] = new PathosNativeFunction(1);
  globals["*"] = new PathosNativeFunction(2);
  globals["/"] = new PathosNativeFunction(3);
  globals["quote"] = new PathosNativeFunction(4);
  globals["lambda"] = new PathosNativeFunction(5);
  globals["if"] = new PathosNativeFunction(6);
  globals["="] = new PathosNativeFunction(7);
  ContainerNode* parsed = parse(load(args[1]));
  cout << parsed->toString() << endl;
  PathosAtom* added = (new PathosUninterpretedList(parsed, &globals))->eval();
  cout << added->toString() << endl;
}
