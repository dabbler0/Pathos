/*
  Copyright (c) 2012 Anthony Bau
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission 
  notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "Pathos.h"
#include "Nodes.h"
using namespace std;

//PathosAtom
int PathosAtom::getType() const {
  return type;
}

string PathosAtom::toString() {
  stringstream out;
  out << "ATOM TYPE " << type << " AT " << this;
  return out.str();
}

//PathosInt
PathosInt::PathosInt(int value) {
  this->value = value;
  this->type = 0;
}
int PathosInt::getValue() const {
  return value;
}
string PathosInt::toString() {
  stringstream out;
  out << value;
  return out.str();
}

//PathosString
PathosString::PathosString(string value) {
  this->value = value;
  this->type = 1;
}
string PathosString::getValue() const {
  return value;
}
string PathosString::toString() {
  return value;
}

//PathosBoolean
PathosBoolean::PathosBoolean(bool value) {
  this->value = value;
  this->type = 2;
}

bool PathosBoolean::getValue() const {
  return value;
}

string PathosBoolean::toString() {
  return (value ? "#t" : "#f");
}

//PathosList
PathosList::PathosList(vector<PathosAtom*> value) {
  this->value = value;
  this->size = value.size();
  this->type = 3;
}

vector<PathosAtom*> PathosList::getValue() const {
  return value;
}

PathosAtom* PathosList::getCar() {
  if (size == 0) {
    cout << "Empty list has no 'car'." << endl;
    throw 2;
  }
  return value[0];
}

PathosAtom* PathosList::getCdr() {
  if (size == 0) {
    cout << "Empty list has no 'cdr'." << endl;
    throw 2;
  }
  vector<PathosAtom*> rtn;

  for (int i = 1; i < size; i += 1) {
    rtn.push_back(value[i]);
  }

  return new PathosList(rtn);
}

PathosAtom* PathosList::consTo(PathosAtom* what) {
  vector<PathosAtom*> rtn;
  rtn.push_back(what);
  for (int i = 0; i < size; i += 1) {
    rtn.push_back(value[i]);
  }
  return new PathosList(rtn);
}

PathosAtom* PathosList::appendTo(PathosAtom* what) {
  vector<PathosAtom*> rtn;
  for (int i = 0; i < size; i += 1) {
    rtn.push_back(value[i]);
  }
  rtn.push_back(what);
  return new PathosList(rtn);
}

bool PathosList::isEmpty() {
  return size == 0;
}

string PathosList::toString() {
  stringstream out;
  out << '[';
  for (int i = 0; i < size; i += 1) {
    out << value[i]->toString();
    if (i != size - 1) out << ", ";
  }
  out << ']';
  return out.str();
}

//PathosUninterpreted

unordered_map<string, PathosAtom*>* PathosUninterpreted::getClosure() {
  return closure;
}

//PathosUninterpretedText
PathosUninterpretedText::PathosUninterpretedText (string text, unordered_map<string, PathosAtom*>* variables) {
  closure = variables;
  value = text;
  type = 4;
}

PathosAtom* PathosUninterpretedText::eval(unordered_map<string, PathosAtom*>* variables) {

  if (value[0] == '\'') {
    //Attempt to interpret as a string literal:
    return new PathosString(value.substr(1, value.length()));
  }
  else if (value[0] == '#') {
    //Attempt to interpret as a boolean literal:
    return new PathosBoolean(value[1] == 't');
  }
  else if (value[0] == '-' && isdigit(value[1]) || isdigit(value[0])) {
    //Attempt to interpret as an int literal:
    return new PathosInt(stoi(value));
  }
  else if (variables->count(value) > 0) {
    //Attempt to dereference from the local variables:
    return variables->at(value);
  }
  else if (closure->count(value) > 0) {
    //Attempt to dereference from the closure:
    return closure->at(value);
  }
  else {
    //If none of the above worked, throw an error.
    cout << "Error: could not dereference variable " << value << '.' << endl;
    throw 0;
  }
}

PathosAtom* PathosUninterpretedText::eval() {
  //Evaluate as normal with an empty map for variables.
  return eval(new unordered_map<string, PathosAtom*>());
}

//PathosUninterpretedList
PathosUninterpretedList::PathosUninterpretedList (ContainerNode* node, unordered_map<string, PathosAtom*>* variables) {
  closure = variables;
  parts = node->getParts();
  size = parts.size();
  type = 5;
}

PathosAtom* PathosUninterpretedList::eval(unordered_map<string, PathosAtom*>* variables) {
  //Get our parts as a bunch of PathosUninterpreted objects:
  vector<PathosUninterpreted*> pathosParts = getParts(variables);

  //Get our function by evaling the first part:
  PathosCallable* function = dynamic_cast<PathosCallable*>(pathosParts[0]->eval());

  if (function == 0) {
    cout << pathosParts[0]->eval()->toString() << " is not callable." << endl;
    throw 5;
  }

  //Assemble our arguments from the others:
  vector<PathosUninterpreted*> args;
  for (int i = 1; i < size; i += 1) {
    args.push_back(pathosParts[i]);
  }

  //Call our function on our arguments:
  return function->call(args);
}

PathosAtom* PathosUninterpretedList::eval() {
  //As above.
  return eval(new unordered_map<string, PathosAtom*>());
}   

vector<PathosUninterpreted*> PathosUninterpretedList::getParts(unordered_map<string, PathosAtom*>* variables) {
  //Construct a new closure from our closure and the new variables.
  unordered_map<string, PathosAtom*>* newClosure = new unordered_map<string, PathosAtom*>;

  //Loop through our closure keys and add them to the new closure:
  unordered_map<string, PathosAtom*>::iterator closureEnd = closure->end();

  for (unordered_map<string, PathosAtom*>::iterator i = closure->begin(); i != closureEnd; ++i) {
    newClosure->operator[](i->first) = i->second;
  }

  //Loop through our variable keys and add them to the new closure:
  unordered_map<string, PathosAtom*>::iterator variableEnd = variables->end();
  for (unordered_map<string, PathosAtom*>::iterator i = variables->begin(); i != variableEnd; ++i) {
    newClosure->operator[](i->first) =i->second;
  }

  //Now assemble the new parts:
  vector<PathosUninterpreted*> rtn;
  for (int i = 0; i < size; i += 1) {
    //Instantiate a new PathosUninterpreted object for each node:
    if (parts[i]->isContainer()) {
      rtn.push_back(new PathosUninterpretedList(dynamic_cast<ContainerNode*>(parts[i]), newClosure));
    }
    else {
      string text = dynamic_cast<TextNode*>(parts[i])->getValue();
      rtn.push_back(new PathosUninterpretedText(text, newClosure));
    }
  }

  return rtn;
}
vector<Node*> PathosUninterpretedList::getRawParts() {
  return parts;
}

//PathosLambda
PathosLambda::PathosLambda (vector<string> params, PathosUninterpreted* value) {
  this->value = value;
  this->params = params;
  this->argSize = params.size();
  this->type = 6;
}

PathosAtom* PathosLambda::call(vector<PathosUninterpreted*> args) {
  //We will construct a map of argument strings to their values:
  unordered_map<string, PathosAtom*> argMap = *(new unordered_map<string, PathosAtom*>());

  if (args.size() != argSize) {
    cout << "Error: function takes " << argSize << " arguments (";
    for (int i = 0; i < argSize; i += 1) {
      cout << params[i];
      if (i != argSize - 1) cout << ' ';
    }
    cout << "), but was called on " << args.size() << " arguments." << endl;
    throw 1;
  }

  for (int i = 0; i < argSize; i += 1) {
    if (params[i][0] == '^') {
      //An argument name beginning with a carat signifies a request for a uninterpreted Pathos tree.
      argMap[params[i]] = args[i];
    }
    else {
      //Otherwise, immediately evaluate the argument.
      argMap[params[i]] = args[i]->eval();
    }
  }
  //Evaluate our function body with these arguments as extra variables.
  return value->eval(&argMap);
}
//END OF PathosLambda METHODS.

//PathosNativeFunction
PathosNativeFunction::PathosNativeFunction (int which) {
  this->which = which;
  this->type = 7;
}

PathosAtom* PathosNativeFunction::call(vector<PathosUninterpreted*> args) {
  return NativeFunctions::call(which, args);
}

//NativeFunctions
//Method to see whether we consider two atoms "equal":
bool equal(PathosAtom* a, PathosAtom* b) {
  if (a->getType() == b->getType()) {
    switch (a->getType()) {
      case 0:
        return (dynamic_cast<PathosInt*>(a)->getValue() == dynamic_cast<PathosInt*>(b)->getValue());
        break;
      case 1:
        return (dynamic_cast<PathosString*>(a)->getValue() == dynamic_cast<PathosString*>(b)->getValue());
        break;
      case 2:
        return (dynamic_cast<PathosBoolean*>(a)->getValue() == dynamic_cast<PathosBoolean*>(b)->getValue());
        break;
      case 3:
        {
          vector<PathosAtom*> first = dynamic_cast<PathosList*>(a)->getValue();
          vector<PathosAtom*> second = dynamic_cast<PathosList*>(b)->getValue();
          int size = first.size();
          if (second.size() != size) {
            return false;
          }
          else {
            for (int i = 0; i < size; i += 1) {
              if (!equal(first[i], second[i])) return false;
            }
            return true;
          }
        }
        break;
      default:
        return a == b;
    }
  }
  return false;
}

inline void assertArgs(const vector<PathosUninterpreted*>& args, int size, string name) {
  if (args.size() != size) {
    cout << name << " takes " << size << " arguments, but was given " << args.size() << '.' << endl;
    throw 1;
  }
}

PathosAtom* NativeFunctions::add(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "+");
  return new PathosInt(dynamic_cast<PathosInt*>(args[0]->eval())->getValue() +
                       dynamic_cast<PathosInt*>(args[1]->eval())->getValue());
}
PathosAtom* NativeFunctions::subtract(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "-");
  return new PathosInt(dynamic_cast<PathosInt*>(args[0]->eval())->getValue() -
                       dynamic_cast<PathosInt*>(args[1]->eval())->getValue());
}
PathosAtom* NativeFunctions::multiply(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "*");
  return new PathosInt(dynamic_cast<PathosInt*>(args[0]->eval())->getValue() *
                       dynamic_cast<PathosInt*>(args[1]->eval())->getValue());
}
PathosAtom* NativeFunctions::divide(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "/");
  return new PathosInt(dynamic_cast<PathosInt*>(args[0]->eval())->getValue() /
                       dynamic_cast<PathosInt*>(args[1]->eval())->getValue());
}
PathosAtom* NativeFunctions::quote(vector<PathosUninterpreted*> args) {
  vector<PathosAtom*> value;
  int size = args.size();
  for (int i = 0; i < size; i += 1) {
    value.push_back(args[i]->eval());
  }
  return new PathosList(value);
}
PathosAtom* NativeFunctions::lambda(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "lambda");
  vector<string> params;
  vector<Node*> parts = dynamic_cast<PathosUninterpretedList*>(args[0])->getRawParts();
  int size = parts.size();
  for (int i = 0; i < size; i += 1) {
    params.push_back(dynamic_cast<TextNode*>(parts[i])->getValue());
  }
  return new PathosLambda(params, args[1]);
}
PathosAtom* NativeFunctions::if_else(vector<PathosUninterpreted*> args) {
  assertArgs(args, 3, "if");
  if (dynamic_cast<PathosBoolean*>(args[0]->eval())->getValue()) {
    return args[1]->eval();
  }
  else {
    return args[2]->eval();
  }
}
PathosAtom* NativeFunctions::equals(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "=");
  return new PathosBoolean(equal(args[0]->eval(),args[1]->eval()));
}
PathosAtom* NativeFunctions::car(vector<PathosUninterpreted*> args) {
  assertArgs(args, 1, "car");
  return dynamic_cast<PathosList*>(args[0]->eval())->getCar();
}
PathosAtom* NativeFunctions::cdr(vector<PathosUninterpreted*> args) {
  assertArgs(args, 1, "cdr");
  return dynamic_cast<PathosList*>(args[0]->eval())->getCdr();
}
PathosAtom* NativeFunctions::cons(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "cons");
  return dynamic_cast<PathosList*>(args[1]->eval())->consTo(args[0]->eval());
}
PathosAtom* NativeFunctions::empty(vector<PathosUninterpreted*> args) {
  assertArgs(args, 1, "empty");
  return new PathosBoolean(dynamic_cast<PathosList*>(args[0]->eval())->isEmpty());
}
PathosAtom* NativeFunctions::append(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "append");
  return dynamic_cast<PathosList*>(args[1]->eval())->appendTo(args[0]->eval());
}
PathosAtom* NativeFunctions::get(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "get");
  return dynamic_cast<PathosList*>(args[0]->eval())->getValue()[dynamic_cast<PathosInt*>(args[1]->eval())->getValue()];
}
PathosAtom* NativeFunctions::log(vector<PathosUninterpreted*> args) {
  assertArgs(args, 2, "log");
  cout << args[0]->eval()->toString() << endl;
  return args[1]->eval();
}
PathosAtom* NativeFunctions::std_in(vector<PathosUninterpreted*> args) {
  assertArgs(args, 0, "stdin");
  string in;
  getline(cin, in);
  Node* parsed = ParserFunctions::parse(in);
  PathosUninterpreted* uninterpreted;
  if (parsed->isContainer()) {
    uninterpreted = new PathosUninterpretedList(dynamic_cast<ContainerNode*>(parsed), getStandardGlobals());
  }
  else {
    uninterpreted = new PathosUninterpretedText(dynamic_cast<TextNode*>(parsed)->getValue(), getStandardGlobals());
  }
  PathosAtom* rtn = uninterpreted->eval();
  //delete uninterpreted
  return rtn;
}
PathosAtom* NativeFunctions::import(vector<PathosUninterpreted*> args) {
  assertArgs(args, 1, "import");
  string in = ParserFunctions::load(dynamic_cast<PathosString*>(args[0]->eval())->getValue());
  Node* parsed = ParserFunctions::parse(in);
  PathosUninterpreted* uninterpreted;
  if (parsed->isContainer()) {
    uninterpreted = new PathosUninterpretedList(dynamic_cast<ContainerNode*>(parsed), getStandardGlobals());
  }
  else {
    uninterpreted = new PathosUninterpretedText(dynamic_cast<TextNode*>(parsed)->getValue(), getStandardGlobals());
  }
  PathosAtom* rtn = uninterpreted->eval();
  //delete uninterpreted;
  return rtn;
}
PathosAtom* NativeFunctions::load(vector<PathosUninterpreted*> args) {
  assertArgs(args, 1, "load");
  return new PathosString(ParserFunctions::load(dynamic_cast<PathosString*>(args[1]->eval())->getValue()));
}
PathosAtom* NativeFunctions::call(int which, vector<PathosUninterpreted*> args) {
  switch (which) {
    case 0:
      return add(args);
      break;
    case 1:
      return subtract(args);
      break;
    case 2:
      return multiply(args);
      break;
    case 3:
      return divide(args);
      break;
    case 4:
      return quote(args);
      break;
    case 5:
      return lambda(args);
      break;
    case 6:
      return if_else(args);
      break;
    case 7:
      return equals(args);
      break;
    case 8:
      return car(args);
      break;
    case 9:
      return cdr(args);
      break;
    case 10:
      return cons(args);
      break;
    case 11:
      return empty(args);
      break;
    case 12:
      return append(args);
      break;
    case 13:
      return get(args);
      break;
    case 14:
      return log(args);
      break;
    case 15:
      return std_in(args);
      break;
    case 16:
      return import(args);
      break;
    case 17:
      return load(args);
      break;
  }
}
unordered_map<string, PathosAtom*>* NativeFunctions::getStandardGlobals() {
  unordered_map<string, PathosAtom*>* globals = new unordered_map<string, PathosAtom*>();
  globals->operator[]("+") = new PathosNativeFunction(0);
  globals->operator[]("-") = new PathosNativeFunction(1);
  globals->operator[]("*") = new PathosNativeFunction(2);
  globals->operator[]("/") = new PathosNativeFunction(3);
  globals->operator[]("quote") = new PathosNativeFunction(4);
  globals->operator[]("lambda") = new PathosNativeFunction(5);
  globals->operator[]("if") = new PathosNativeFunction(6);
  globals->operator[]("=") = new PathosNativeFunction(7);
  globals->operator[]("car") = new PathosNativeFunction(8);
  globals->operator[]("cdr") = new PathosNativeFunction(9);
  globals->operator[]("cons") = new PathosNativeFunction(10);
  globals->operator[]("empty") = new PathosNativeFunction(11);
  globals->operator[]("append") = new PathosNativeFunction(12);
  globals->operator[]("get") = new PathosNativeFunction(13);
  globals->operator[]("log") = new PathosNativeFunction(14);
  globals->operator[]("stdin") = new PathosNativeFunction(15);
  globals->operator[]("import") = new PathosNativeFunction(16);
  globals->operator[]("load") = new PathosNativeFunction(17);
  return globals;
}

//Pathos
PathosAtom* Pathos::interpret(string pathos) {
  Node* parsed = ParserFunctions::parse(pathos);
  PathosUninterpreted* uninterpreted;
  if (parsed->isContainer()) {
    uninterpreted = new PathosUninterpretedList(dynamic_cast<ContainerNode*>(parsed), NativeFunctions::getStandardGlobals());
  }
  else {
    uninterpreted = new PathosUninterpretedText(dynamic_cast<TextNode*>(parsed)->getValue(), NativeFunctions::getStandardGlobals());
  }
  return uninterpreted->eval();
}
