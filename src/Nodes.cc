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
#include <vector>
#include <unordered_map>
#include <sstream>
#include "Nodes.h"
using namespace std;

//Node
string Node::toString() const {
  ostringstream rtn;
  rtn << "{NODE parent@" << parent << "}";
  return rtn.str();
};

bool Node::isContainer() const {
  return false;
}

bool Node::isRoot() const {
  return root;
}

ContainerNode* Node::consummate() {
  parent->addPart(this);
  return parent;
}

//ContainerNode
ContainerNode::ContainerNode (ContainerNode* parent) {
  this->parent = parent;
  this->root = false;
}

ContainerNode::ContainerNode () {
  this->parent = 0;
  this->root = true;
}

vector<Node*> ContainerNode::getParts() const {
  return parts;
}

void ContainerNode::addPart (Node* node) {
  parts.push_back(node);
}

string ContainerNode::toString() const {
  string rtn = "(";
  int len = parts.size();
  for (int i = 0; i < len; i += 1) {
    rtn += parts[i]->toString();
    if (i != len - 1) rtn += ' ';
  }
  rtn += ")";
  return rtn;
}

bool ContainerNode::isContainer() const {
  return true;
}

//TextNode
TextNode::TextNode (ContainerNode* parent, string value) {
  this->parent = parent;
  this->value = value;
  this->root = false;
}

string TextNode::getValue() const {
  return value;
}

string TextNode::toString() const {
  return value;
}

bool TextNode::isContainer() const {
  return false;
}

//Parse.
Node* ParserFunctions::parse(string pathos) {
  int len = pathos.length();
  ContainerNode* current = new ContainerNode();
  string last = "";
  int x = 0;
  if (pathos[0] == '#' && pathos[1] == '!') {
    //If there is an interpreter line, ignore it.
    while (pathos[x] != '\n') {
      x += 1;
    }
  }
  for (int i = x; i < len; i += 1) {
    if (pathos[i] == '(') {
      current = new ContainerNode(current);
    }
    else if (pathos[i] == ')') {
      if (last.length() > 0) {
        current->addPart(new TextNode(current, last));
        last = "";
      }
      if (current->isRoot()) {
        cout << "Mismatched parenthesis." << endl;
        throw 4;
      }
      current = current->consummate();
    }
    else if (pathos[i] == ' ') {
      if (last.length() > 0) {
        current->addPart(new TextNode(current, last));
        last = "";
      }
    }
    else if (pathos[i] == '\n') {
      //We will disregard line breaks.
    }
    else if (pathos[i] == '/' && pathos[i + 1] == '/') {
      //Comments are c-like.
      while (pathos[i] != '\n') {
        i += 1;
      }
    }
    else if (pathos[i] == '\\') {
      //Backslash is a universal escape character.
      i += 1;
      last += pathos[i];
    }
    else {
      last += pathos[i];
    }
  }
  
  if (last.length() > 0) current->addPart(new TextNode(current, last));

  return current->getParts()[0];
}

//Load.
string ParserFunctions::load(string where) {
  ifstream file (where);
  stringstream str;
  str << file.rdbuf();
  return str.str();
}
