/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
int Globalref = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   cout << "==================================================" << endl;

   string out;
   out.append("= ");
   out.append(getTypeStr());
   out.append("(");
   out.append(to_string(id));
   out.append(")");
   if(!name.empty()) {
      out.append("\"");
      out.append(name);
      out.append("\"");
   }
   out.append(", line ");
   out.append(to_string(lineNo));
   cout << setw(49) << left << out << "=" << endl;
   // cout << "faninlist:";
   // for(size_t i = 0; i < _faninList.size(); i++){
   //    cout << " " << _faninList[i].gate()->getId();
   // }
   // cout << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   Globalref++;
   cout << getTypeStr() << " " << id << endl;
   dfsin(1, level);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   Globalref++;
   cout << getTypeStr() ;
   if(getTypeStr() != "CONST0") cout << " ";
   cout << id << endl;
   dfsout(1, level);
}


void 
CirGate::dfsin(int j, int level) const
{
   setGlobalref();
   for(size_t i = 0; i < _faninList.size(); i++){
      for(int k = 0; k < j; k++){
         cout << "  ";
      }
      if(_faninList[i].isInv()) cout << "!" ;
      cout << _faninList[i].gate()->getTypeStr();
      cout << " " << _faninList[i].gate()->id;
      if(_faninList[i].gate()->ref == Globalref && j+1 <= level && !_faninList[i].gate()->_faninList.empty())   cout << " (*)" << endl;
      else {
         cout << endl;
         if(j+1 <= level) _faninList[i].gate()->dfsin(j+1, level);
      }
   }
}

void 
CirGate::dfsout(int j, int level) const
{
   setGlobalref();
   for(size_t i = 0; i < _fanoutList.size(); i++){
      for(int k = 0; k < j; k++){
         cout << "  ";
      }
      if(_fanoutList[i].isInv()) cout << "!" ;
      cout << _fanoutList[i].gate()->getTypeStr();
      cout << " " << _fanoutList[i].gate()->id;
      if(_fanoutList[i].gate()->ref == Globalref && j+1 <= level && !_fanoutList[i].gate()->_fanoutList.empty())   cout << " (*)" << endl;
      else {
         cout << endl;
         if(j+1 <= level) _fanoutList[i].gate()->dfsout(j+1, level);
      }
   }
}

void
CirGate::dfsnet(unsigned& LineNo) const
{
   setGlobalref();
   if(getTypeStr() == "UNDEF")   return;
   for(size_t i = 0; i < _faninList.size(); i++){
      if(_faninList[i].gate()->ref != Globalref)   _faninList[i].gate()->dfsnet(LineNo);
   }
   cout << "[" << LineNo << "] " << setw(3) << left << getTypeStr();
   if(getTypeStr() != "CONST") cout << " ";
   cout << id;
   for(size_t i = 0; i < _faninList.size(); i++){
      cout << " ";
      if(_faninList[i].gate()->getTypeStr() == "UNDEF")  cout << "*";
      if(_faninList[i].isInv()) cout << "!";
      cout << _faninList[i].gate()->getId();
   }
   if(!name.empty()) cout << " (" << name << ")" << endl;
   else cout << endl;
   LineNo++;
}