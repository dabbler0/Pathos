#include <iostream>
#include <vector>
#include "Nodes.h"
#include "Pathos.h"
using namespace std;

int main(int n, char* args[]) {
  cout << Pathos::interpret(ParserFunctions::load(args[1]))->toString() << endl;
}
