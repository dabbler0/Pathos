#include <vector>
#include <unordered_map>
#include "Nodes.h"
using namespace std;

#ifndef __PATHOS_ATOMS_DECLARED__
#define __PATHOS_ATOMS_DECLARED__

class PathosAtom {
  protected:
    int type;
  public:
    int getType() const;
    virtual string toString();
};

class PathosInt : public PathosAtom {
  private:
    int value;
  public:
    PathosInt(int);
    int getValue() const;
    string toString();
};

class PathosString : public PathosAtom {
  private:
    string value;
  public:
    PathosString(string);
    string getValue() const;
    string toString();
};

class PathosBoolean : public PathosAtom {
  private:
    bool value;
  public:
    PathosBoolean(bool);
    bool getValue() const;
    string toString();
};

class PathosList : public PathosAtom {
  private:
    vector<PathosAtom*> value;
    int size;

  public:
    PathosList(vector<PathosAtom*>);
    vector<PathosAtom*> getValue() const;
    PathosAtom* getCar();
    PathosAtom* getCdr();
    PathosAtom* consTo(PathosAtom*);
    PathosAtom* appendTo(PathosAtom*);
    bool isEmpty();
    string toString();
};

class PathosUninterpreted : public PathosAtom {
  protected:
    unordered_map<string, PathosAtom*>* closure;
  public:
    virtual PathosAtom* eval(unordered_map<string, PathosAtom*>* variables) =0;
    virtual PathosAtom* eval() =0;
    unordered_map<string, PathosAtom*>* getClosure();
};

class PathosCallable : public PathosAtom {
  public:
    virtual PathosAtom* call(vector<PathosUninterpreted*>) =0;
};

class PathosUninterpretedText : public PathosUninterpreted {
  private:
    string value;
  public:
    PathosUninterpretedText (string, unordered_map<string, PathosAtom*>*);
    PathosAtom* eval(unordered_map<string, PathosAtom*>*);
    PathosAtom* eval();
};

class PathosUninterpretedList : public PathosUninterpreted {
  private:
    vector<Node*> parts;
    int size;
  public:
    PathosUninterpretedList (ContainerNode*, unordered_map<string, PathosAtom*>*);
    vector<PathosUninterpreted*> getParts(unordered_map<string, PathosAtom*>*);
    vector<PathosUninterpreted*> getParts();
    vector<Node*> getRawParts();
    PathosAtom* eval(unordered_map<string, PathosAtom*>*);
    PathosAtom* eval();
};

class PathosLambda : public PathosCallable {
  private:
    PathosUninterpreted* value;
    vector<string> params;
    int argSize;
  public:
    PathosLambda(vector<string>, PathosUninterpreted*);
    PathosAtom* call(vector<PathosUninterpreted*>);
};

class PathosNativeFunction : public PathosCallable {
  private:
    int which;
  public:
    PathosNativeFunction(int);
    PathosAtom* call(vector<PathosUninterpreted*>);
};

namespace NativeFunctions {
  PathosAtom* add(vector<PathosUninterpreted*>);
  PathosAtom* subtract(vector<PathosUninterpreted*>);
  PathosAtom* multiply(vector<PathosUninterpreted*>);
  PathosAtom* divide(vector<PathosUninterpreted*>);
  PathosAtom* lambda(vector<PathosUninterpreted*>);
  PathosAtom* quote(vector<PathosUninterpreted*>);
  PathosAtom* if_else(vector<PathosUninterpreted*>);
  PathosAtom* equals(vector<PathosUninterpreted*>);
  PathosAtom* car(vector<PathosUninterpreted*>);
  PathosAtom* cdr(vector<PathosUninterpreted*>);
  PathosAtom* cons(vector<PathosUninterpreted*>);
  PathosAtom* append(vector<PathosUninterpreted*>);
  PathosAtom* get(vector<PathosUninterpreted*>);
  PathosAtom* empty(vector<PathosUninterpreted*>);
  PathosAtom* log(vector<PathosUninterpreted*>);
  PathosAtom* call(int, vector<PathosUninterpreted*>);
}

#endif
