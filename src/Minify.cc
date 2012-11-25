#include <iostream>
#include <fstream>
#include "Nodes.h"

int main(int n, char* args[]) {
  ofstream outfile (args[2]);
  outfile << ParserFunctions::parse(ParserFunctions::load(args[1]))->toString();
  return 0;
}
