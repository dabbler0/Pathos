#include <iostream>
#include <vector>
#include "Nodes.h"
#include "Pathos.h"
using namespace std;

int main(int n, char* args[]) {
  unordered_map<string, PathosAtom*> globals;
  globals["+"] = new PathosNativeFunction(0);
  globals["="] = new PathosNativeFunction(7);
  globals["lambda"] = new PathosNativeFunction(5);
  globals["if"] = new PathosNativeFunction(6);
  ContainerNode* parsed = parse(load(args[1]));
  cout << "Loaded " << parsed->toString() << endl;
  PathosAtom* added = (new PathosUninterpretedList(parsed, &globals))->eval();
  cout << added->toString() << endl;
}
