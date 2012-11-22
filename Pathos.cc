#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
using namespace std;

//PARSING PATHOS TREES.

class Node;
class ContainerNode;
class TextNode;

class Node {
  protected:
    ContainerNode* parent;

  public:
    ContainerNode* consummate ();
    
    template<class T>
    T considerAs() {
      T* considered = dynamic_cast<T*>(this);
      if (considered == NULL) {
        throw 1;
      }
      else {
        return *considered;
      }
    }

    virtual string toString() const {
      ostringstream rtn;
      rtn << "{NODE parent@" << parent << "}";
      return rtn.str();
    };

    virtual bool isContainer() const {
      return false;
    }
};

class ContainerNode : public Node {
  public:
    vector<Node*> parts;

    ContainerNode (ContainerNode* parent) {
      this->parent = parent;
    }

    ContainerNode () {
    }
    
    void addPart (Node* node) {
      parts.push_back(node);
    }
    
    string toString() const {
      string rtn = "[";
      int len = parts.size();
      for (int i = 0; i < len; i += 1) {
        rtn += parts[i]->toString();
        if (i != len - 1) rtn += ' ';
      }
      rtn += "]";
      return rtn;
    }

    bool isContainer() const {
      return true;
    }
};

class TextNode : public Node {
  public:
    string value;
    TextNode (ContainerNode* parent, string value) {
      this->parent = parent;
      this->value = value;
    }

    string toString() const {
      return value;
    }

    bool isContainer() const {
      return false;
    }
};

ContainerNode* Node::consummate() {
  parent->addPart(this);
  return parent;
}


ContainerNode parse(string pathos) {
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
    else {
      last += pathos[i];
    }
  }
  return *(dynamic_cast<ContainerNode*>(current->parts[0]));
}

//EVALUATING PATHOS TREES.
//TODO: Complete rewrite of the closure system. It's all out of whack.

class PathosAtom {
  protected:
    int type;
    
  public:
    int getType() const {
      return type;
    }
    
    virtual string toString() {
      stringstream out;
      out << "ATOM TYPE " << type << " AT " << this;
      return out.str();
    }
};

PathosAtom& evaluate(const Node&, const unordered_map<string, PathosAtom*>*);

class PathosInt : public PathosAtom {
  private:
    int value;

  public:
    PathosInt(int value) {
      this->value = value;
      this->type = 0;
    }
    
    int getValue() const {
      return value;
    }

    string toString() {
      stringstream out;
      out << value;
      return out.str();
    }
};

class PathosString : public PathosAtom {
  private:
    string value;
  
  public:
    PathosString(string value) {
      this->value = value;
      this->type = 1;
    }

    string getValue() const {
      return value;
    }

    string toString() {
      return value;
    }
};

class PathosBoolean : public PathosAtom {
  private:
    bool value;
  public:
    PathosBoolean(bool value) {
      this->value = value;
      this->type = 2;
    }

    bool getValue() const {
      return value;
    }

    string toString() const {
      return (value ? "true" : "false");
    }
};

class PathosList : public PathosAtom {
  private:
    vector<PathosAtom*> value;
    int size;

  public:
    PathosList(vector<PathosAtom*> value) {
      this->value = value;
      this->size = value.size();
    }

    vector<PathosAtom*> getValue() const {
      return value;
    }

    PathosAtom& getCar() {
      return *value[0];
    }

    PathosAtom& getCdr() {
      vector<PathosAtom*> rtn;
      for (int i = 1; i < size; i += 1) {
        rtn.push_back(value[i]);
      }
      return *(new PathosList(rtn));
    }

    string toString() {
      stringstream out;
      out << '[';
      for (int i = 0; i < size; i += 1) {
        out << value[i]->toString();
        if (i != size - 1) out << ", ";
      }
      out << ']';
      return out.str();
    }
};

class PathosUninterpreted : public PathosAtom {
  private:
    Node* value;
    const unordered_map<string, PathosAtom*>* closure;
  public:
    PathosUninterpreted () {
      this->value = 0;
      this->closure = 0;
    }

    PathosUninterpreted(const Node& value, const unordered_map<string, PathosAtom*>* closure) {
      this->value = &const_cast<Node&>(value);
      this->closure = closure;
      for (unordered_map<string, PathosAtom*>::const_iterator x = this->closure->begin(); x != this->closure->end(); ++x) {
        cout << "An element of the new closure is (" << x->first << ", " << x->second->toString() << ")\n";
      }
    }

    Node& getValue() const {
      return *value;
    }

    const unordered_map<string, PathosAtom*>* getClosure() const {
      return closure;
    }

    PathosAtom& eval(const unordered_map<string, PathosAtom*>& variables) const {
      cout << "Beginning uninterpreted evaluation.\n";
      const unordered_map<string, PathosAtom*>* random = closure;
      cout << closure << endl;
      unordered_map<string, PathosAtom*>::const_iterator iterator = closure->begin();
      while (iterator != closure->end()) {
        cout << "An element of the closure is " << iterator->first << '\n';
        ++iterator;
      }
      unordered_map<string, PathosAtom*> ultimateVariables = *(new unordered_map<string, PathosAtom*>());
      for (unordered_map<string, PathosAtom*>::const_iterator x = closure->begin(); x != closure->end(); ++x) {
        ultimateVariables[x->first] = x->second;
        cout << "Adding closure variable " << x->first << " to ultimate variables.\n";
      }
      cout << "Constructed ultimate variables.\n";
      for (unordered_map<string, PathosAtom*>::const_iterator x = variables.begin(); x != variables.end(); ++x) {
        ultimateVariables[x->first] = x->second;
        cout << "Meshing local variable " << x->first << " with ultimate variables.\n";
      }
      cout << "Done meshing vars.\n";
      return evaluate(*value, &ultimateVariables);
    }

    PathosAtom& eval() const {
      return evaluate(*value, closure);
    }
};

class PathosLambda : public PathosAtom {
  private:
    int id;
    PathosUninterpreted value;
    vector<string> params;
    int size;
    
  public:
    PathosLambda () {
      this->type = 7;
    }

    PathosLambda(const PathosUninterpreted& value, const vector<string>& params) {
      static int gid;
      this->id = (gid += 1);
      cout << "Constructed lambda with id " << this->id << ".\n";
      this->value = value;
      this->params = params;
      this->size = params.size();
      this->type = 7;
    }

    PathosUninterpreted& getValue() {
      return value;
    }
    
    vector<string> getArgs() {
      return params;
    }

    virtual PathosAtom& run(const vector<PathosUninterpreted>& args) {
      cout << "Running a function.\n";
      unordered_map<string, PathosAtom*> argMap;
      for (int i = 0; i < size; i += 1) {
        if (params[i][0] == '^') {
          argMap[params[i]] = const_cast<PathosAtom*>(dynamic_cast<const PathosAtom*>(&args[i]));
        }
        else {
          argMap[params[i]] = const_cast<PathosAtom*>(dynamic_cast<const PathosAtom*>(&args[i].eval()));
        }
      }
      cout << "Done constructing argMap for function " << id << ".\n";
      return value.eval(argMap);
    }
};

namespace NativeFunctions {
  PathosAtom& add(PathosAtom& a, PathosAtom& b) {
    cout << "Doing addition.\n";
    PathosInt& intA = dynamic_cast<PathosInt&>(a);
    PathosInt& intB = dynamic_cast<PathosInt&>(b);
    return *(new PathosInt(intA.getValue() + intB.getValue()));
  }
  PathosAtom& lambda(const PathosUninterpreted& args, const PathosUninterpreted& value) {
    cout << "Making a lambda.\n";
    ContainerNode& argNode = dynamic_cast<ContainerNode&>(args.getValue());
    vector<Node*> paramNodes = argNode.parts;
    int size = paramNodes.size();
    vector<string> params;
    for (int i = 0; i < size; i += 1) {
      params.push_back(dynamic_cast<TextNode*>(paramNodes[i])->value);
    }
    return *(new PathosLambda(value, params));
  }
  PathosAtom& car(const PathosUninterpreted& list) {
    cout << "Carring.\n";
    return dynamic_cast<PathosList&>(list.eval()).getCar();
  }
  PathosAtom& cdr(const PathosUninterpreted& list) {
    cout << "Cdring.\n";
    return dynamic_cast<PathosList&>(list.eval()).getCdr();
  }
  PathosAtom& eval(const PathosUninterpreted& uninterpreted) {
    cout << "Evaling.\n";
    return dynamic_cast<PathosUninterpreted&>(uninterpreted.eval()).eval();
  }
  PathosAtom& quote(const PathosUninterpreted& list) {
    cout << "Quoting.\n";
    ContainerNode& listNode = dynamic_cast<ContainerNode&>(list.getValue());
    vector<Node*> elementNodes = listNode.parts;
    vector<PathosAtom*> elements;
    int size = elementNodes.size();
    for (int i = 0; i < size; i += 1) {
      elements.push_back(&evaluate(*elementNodes[i], list.getClosure()));
    }
    return *(new PathosList(elements));
  }
  PathosAtom& equals(const PathosUninterpreted& a, const PathosUninterpreted& b) {
    bool ret = false;
    if (a.getType() == 0 && b.getType() == 0) {
      ret = dynamic_cast<const PathosInt&>(a).getValue() == dynamic_cast<const PathosInt&>(b).getValue();
    }
    else if (a.getType() == 1 && b.getType() == 1) {
      ret = dynamic_cast<const PathosString&>(a).getValue().compare(dynamic_cast<const PathosString&>(b).getValue()) == 0;
    }
    else if (a.getType() == 2 && b.getType() == 2) {
      ret = dynamic_cast<const PathosBoolean&>(a).getValue() == dynamic_cast<const PathosBoolean&>(b).getValue();
    }
    return *(new PathosBoolean(ret));
  }
  PathosAtom& ifelse(const PathosUninterpreted& cond, const PathosUninterpreted& then, const PathosUninterpreted& other) {
    cout << "Beginning the if_else.\n";
    if (dynamic_cast<PathosBoolean&>(cond.eval()).getValue()) {
      return then.eval();
    }
    else {
      return other.eval();
    }
  }
};

class PathosNativeFunction : public PathosLambda {
  private:
    int which;
  public:
    PathosNativeFunction (string name) {
      if (name.compare("+") == 0) {
        which = 0;
      }
      else if (name.compare("lambda") == 0) {
        which = 1;
      }
      else if (name.compare("car") == 0) {
        which = 2;
      }
      else if (name.compare("cdr") == 0) {
        which = 3;
      }
      else if (name.compare("quote") == 0) {
        which = 4;
      }
      else if (name.compare("=") == 0) {
        which = 5;
      }
      else if (name.compare("if") == 0) {
        which = 6;
      }
      this->type = 8;
    }

    PathosAtom& run(const vector<PathosUninterpreted>& args) {
      switch (which) {
        case 0:
          return NativeFunctions::add(args[0].eval(), args[1].eval());
          break;
        case 1:
          return NativeFunctions::lambda(args[0], args[1]);
          break;
        case 2:
          return NativeFunctions::car(args[0]);
          break;
        case 3:
          return NativeFunctions::cdr(args[0]);
          break;
        case 4:
          return NativeFunctions::quote(args[0]);
          break;
        case 5:
          return NativeFunctions::equals(args[0], args[1]);
          break;
        case 6:
          return NativeFunctions::ifelse(args[0], args[1], args[2]);
          break;
      }
    }
};

PathosAtom& evaluate(const Node& pathos, const unordered_map<string, PathosAtom*>* variables) {
  cout << pathos.toString() << '\n';
  if (pathos.isContainer()) {
    ContainerNode& container = const_cast<ContainerNode&>(dynamic_cast<const ContainerNode&>(pathos));
    PathosLambda& function = dynamic_cast<PathosLambda&>(evaluate(*container.parts[0], variables));

    vector<PathosUninterpreted> args;
    int size = container.parts.size();

    for (int i = 1; i < size; i += 1) {
      cout << "Added an arg to " << pathos.toString() << '\n';
      args.push_back(*(new PathosUninterpreted(*container.parts[i], variables)));
    }
    
    return function.run(args);
  }
  else {
    TextNode& text = const_cast<TextNode&>(dynamic_cast<const TextNode&>(pathos));
    if (text.value[0] == '\'') {
      return *(new PathosString(text.value.substr(1,text.value.length())));
    }
    else if (text.value[0] == '-' || isdigit(text.value[0])) {
      return *(new PathosInt(stoi(text.value)));
    }
    else if (text.value[0] == '#') {
      return *(new PathosBoolean(text.value[1] == 't'));
    }
    else if (variables->count(text.value) > 0) {
      return *const_cast<PathosAtom*&>(variables->at(text.value));
    }
    else {
      throw 2;
    }
  }
}

string load(string where) {
  ifstream file (where);
  stringstream str;
  str << file.rdbuf();
  return str.str();
}

int main(int n, char* args[]) {
  unordered_map<string, PathosAtom*> natives;
  natives["+"] = new PathosNativeFunction("+");
  natives["lambda"] = new PathosNativeFunction("lambda");
  natives["car"] = new PathosNativeFunction("car");
  natives["cdr"] = new PathosNativeFunction("cdr");
  natives["quote"] = new PathosNativeFunction("quote");
  natives["="] = new PathosNativeFunction("=");
  natives["if"] = new PathosNativeFunction("if");
  string pathos = load(args[1]);
  cout << evaluate(parse(pathos), &natives).toString() << '\n';
}
