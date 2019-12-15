/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <queue>
#include "cirDef.h"

using namespace std;

class CirGate;
class AigGate;
class AigGateV;
extern int Globalref;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
  friend class CirMgr;

public:
  CirGate(unsigned i, unsigned _id):lineNo(i), id(_id), ref(0), floating(false) {}
  virtual ~CirGate() {}

  // Basic access methods
  virtual string getTypeStr() const { return ""; }
  unsigned getLineNo() const { return lineNo; }

  // Printing functions
  virtual void printGate() const = 0;
  void reportGate() const ;
  void reportFanin(int level) const;
  void reportFanout(int level) const;
  void setGlobalref() const { ref = Globalref; }
  unsigned getId() const { return id; }

private:
  unsigned lineNo;
  unsigned id; // var id
  mutable int ref;
  bool floating;
  string name;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist; // literal id

  void dfsin(int, int) const;
  void dfsout(int, int) const;
  void dfsnet(unsigned&) const;
protected:

};

class AigGateV 
{
  friend class CirGate;
  friend class CirMgr;
  friend class AigGate;
  friend class PO;
  friend class PI;
  friend class Constin;

  #define NEG 0x1
  AigGateV(CirGate* g, size_t phase) :
    _gateV(size_t(g) + phase) { }
  CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(NEG)); }
  bool isInv() const { return (_gateV & NEG); }
  void Inv() { _gateV -= NEG; }
  size_t _gateV;
};

class PI: public CirGate
{
  friend class CirMgr;

public:
  PI(unsigned i, unsigned _id):CirGate(i, _id), lineNo(i), id(_id), ref(0), floating(false) {}
  ~PI() {}
  string getTypeStr() const { return "PI"; }
  void setname(const string& n) { name = n; }
  string getname()  {return name;}
  void printGate() const {
    cout << "PI(" << id << ")" << endl;
  }
private:
  unsigned lineNo;
  unsigned id;
  int ref;
  string name;
  bool floating;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist;
};

class PO: public CirGate
{
  friend class CirMgr;

public:
  PO(unsigned i, unsigned _id):CirGate(i, _id), lineNo(i), id(_id), ref(0), floating(false) { }
  ~PO() {}
  string getTypeStr() const { return "PO"; }
  unsigned getLineNo() const { return lineNo; }
  void setname(const string& n) { name = n; }
  string getname()  {return name;}
  void printGate() const {}
  
private:
  unsigned lineNo;
  unsigned id;
  int ref;
  bool floating;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist;
  string name;
};

class AigGate : public CirGate
{
  friend class CirMgr;

public:
  AigGate(unsigned i, unsigned _id):CirGate(i, _id), lineNo(i), id(_id), ref(0), floating(false){ }
  ~AigGate() {}
  string getTypeStr() const { return "AIG"; }
  unsigned getLineNo() const { return lineNo; }
  void printGate() const {  }
private:
  unsigned lineNo;
  unsigned id;
  int ref;
  bool floating;
  string name;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist;
};

class Const0 : public CirGate
{
  friend class CirMgr;

public:
  Const0():CirGate(0, 0), lineNo(0), id(0), ref(0), floating(false){ }
  ~Const0() {}
  string getTypeStr() const { return "CONST"; }
  unsigned getLineNo() const { return lineNo; }
  void printGate() const {  }
private:
  unsigned lineNo;
  unsigned id;
  int ref;
  bool floating;
  string name;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist;
};

class UndefGate : public CirGate
{
  friend class CirMgr;
  UndefGate(unsigned _id):CirGate(0, _id), lineNo(0), id(_id), ref(0), floating(false){ }
  ~UndefGate() {}
  string getTypeStr() const { return "UNDEF"; }
  unsigned getLineNo() const { return lineNo; }
  void printGate() const { }
private:
  unsigned lineNo;
  unsigned id;
  int ref;
  bool floating;
  string name;
  vector<AigGateV> _faninList;
  vector<AigGateV> _fanoutList;
  queue<unsigned> todolist;

};



#endif // CIR_GATE_H
