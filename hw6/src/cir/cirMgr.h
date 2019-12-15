/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      map<unsigned, CirGate*>::const_iterator it;
      it = total_gate.find(gid);
      if(it == total_gate.end())   return 0;
      else return it->second;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool parsedef(const string&);
   bool readPI(const string&, const int&);
   bool readPO(const string&, const int&, const int& id);
   int readGate(const string&, const int&);
   bool readName(const string& str);
   int parsegate(const string&, int*);
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
   int M, I, L, O, A;
   map<unsigned, CirGate*> total_gate;
   vector<string> content, namedef;
   GateList pilist, polist, glist;
   IdList   _inList, _outList, _gateList;
   void dfs(CirGate*, vector<string>&, int&) const;
   size_t GetNotNum(const string&, size_t);
};

#endif // CIR_MGR_H
