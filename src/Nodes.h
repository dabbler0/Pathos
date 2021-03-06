#ifndef __NODES_INCLUDED__
#define __NODES_INCLUDED__

#include <vector>
using namespace std;

class ContainerNode;

class Node {
  protected:
    ContainerNode* parent;
    bool root;
  public:
    ContainerNode* consummate();
    virtual string toString() const;
    virtual bool isContainer() const;
    bool isRoot() const;
};

class ContainerNode : public Node {
  protected:
    vector<Node*> parts;
  public:
    ContainerNode(ContainerNode*);
    ContainerNode();
    vector<Node*> getParts() const;
    void addPart(Node*);
    string toString() const;
    bool isContainer() const;
};

class TextNode : public Node {
  protected:
    string value;
  public:
    TextNode (ContainerNode*, string);
    string getValue() const;
    string toString() const;
    bool isContainer() const;
};

namespace ParserFunctions {
  Node* parse(string);
  string load(string);
}
#endif
