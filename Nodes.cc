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

ContainerNode* Node::consummate() {
  parent->addPart(this);
  return parent;
}

//ContainerNode
ContainerNode::ContainerNode (ContainerNode* parent) {
  this->parent = parent;
}

ContainerNode::ContainerNode () {
}

vector<Node*> ContainerNode::getParts() const {
  return parts;
}

void ContainerNode::addPart (Node* node) {
  parts.push_back(node);
}

string ContainerNode::toString() const {
  string rtn = "[";
  int len = parts.size();
  for (int i = 0; i < len; i += 1) {
    rtn += parts[i]->toString();
    if (i != len - 1) rtn += ' ';
  }
  rtn += "]";
  return rtn;
}

bool ContainerNode::isContainer() const {
  return true;
}

//TextNode
TextNode::TextNode (ContainerNode* parent, string value) {
  this->parent = parent;
  this->value = value;
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
ContainerNode* parse(string pathos) {
  int len = pathos.length();
  ContainerNode* current = new ContainerNode();
  string last = "";
  for (int i = 0; i < len; i += 1) {
    if (pathos[i] == '(') {
      current = new ContainerNode(current);
    }
    else if (pathos[i] == ')') {
      if (last.length() > 0) {
        current->addPart(new TextNode(current, last));
        last = "";
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
    else {
      last += pathos[i];
    }
  }
  return dynamic_cast<ContainerNode*>(current->getParts()[0]);
}
