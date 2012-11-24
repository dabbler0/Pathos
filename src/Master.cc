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
  globals["car"] = new PathosNativeFunction(8);
  globals["cdr"] = new PathosNativeFunction(9);
  globals["cons"] = new PathosNativeFunction(10);
  globals["empty"] = new PathosNativeFunction(11);
  globals["append"] = new PathosNativeFunction(12);
  globals["get"] = new PathosNativeFunction(13);
  globals["log"] = new PathosNativeFunction(14);
  cout << (new PathosUninterpretedList(parse(load(args[1])), &globals))->eval()->toString() << endl;
}
