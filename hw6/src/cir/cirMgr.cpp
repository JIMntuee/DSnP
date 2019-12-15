/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName) 
{
   fstream infile;
   string tmp;
   unsigned LineNo = 1;
   content.push_back(""); // null string for line 0
   infile.open(fileName);
   if(!infile) { 
      cerr <<  "Cannot open design \"" << fileName << "\"!!" <<endl; 
      return false; 
   }
   getline(infile, tmp);

   if(!parsedef(tmp)) return false; // aag def
   content.push_back(tmp);
   glist.reserve(M);
   pilist.reserve(I);
   polist.reserve(O);

   CirGate* newconst = new Const0;
   total_gate.insert(pair<unsigned, CirGate*>(0, newconst));

   for(int i = 0; i < I; ++i){ // inputdef
      LineNo++;
      if(!getline(infile, tmp)) {
         errMsg = "PIs";
         parseError(MISSING_DEF);
         return 0;
      }
      if(!readPI(tmp, LineNo)) return 0;
      content.push_back(tmp);
   }

   for(int i = 0; i < O; ++i) { // outputdef
      LineNo++;
      if(!getline(infile, tmp)) {
         errMsg = "POs";
         parseError(MISSING_DEF);
         return 0;
      }
      if(!readPO(tmp, LineNo, M+i+1))  return 0;
      content.push_back(tmp);
   }

   for(int i = 0; i < A; ++i) {
      LineNo++;
      if(!getline(infile, tmp)) {
         errMsg = "AIG";
         parseError(MISSING_DEF);
         return 0;
      }
      if(tmp.empty()) {
         errMsg = "AIG";
         parseError(MISSING_DEF);
         return 0;
      }
      content.push_back(tmp);
      int state = 0;
      state = readGate(tmp, LineNo);
      if(state == 0) return 0;
      else if(state == 1)  break;
   }

   while(getline(infile, tmp)) { // readname
      if(tmp == "c") break;
      readName(tmp);
   }

   for(map<unsigned, CirGate*>::iterator it = total_gate.begin(); it != total_gate.end(); it++){ // connect circuit
      while(!it->second->todolist.empty()){ // connect until todolist is empty
         unsigned id = it->second->todolist.front();
         map<unsigned, CirGate*>::iterator pos = total_gate.find(id/2);
         if(pos == total_gate.end()) { // undef gate
            CirGate* undef = new UndefGate(id/2);
            total_gate.insert(pair<unsigned, CirGate*>(id/2, undef));
            glist.push_back(undef);
            AigGateV tmp1(undef, id%2);
            AigGateV tmp2(it->second, id%2);
            it->second->_faninList.push_back(tmp1);
            undef->_fanoutList.push_back(tmp2);
            it->second->floating = true;
         }
         else {
            AigGateV tmp1(pos->second, id%2);
            AigGateV tmp2(it->second, id%2);
            it->second->_faninList.push_back(tmp1);
            pos->second->_fanoutList.push_back(tmp2);
            if(pos->second->getTypeStr() == "UNDEF")  it->second->floating = true;
         }
         it->second->todolist.pop();
      }
   }
   
   return true;
}

bool
CirMgr::readPI(const string& str, const int& LineNo)
{
   int input;
   if(!myStr2Int(str, input)) {
      parseError(ILLEGAL_NUM);
      return 0;
   }
   else if(input == 0 || input == 1) {
      parseError(REDEF_CONST);
      return 0;
   }
   else if(find(_inList.begin(), _inList.end(), input/2) != _inList.end()) {
      parseError(REDEF_CONST);
      return 0;
   }
   _inList.push_back(input/2);
   CirGate* newG = new PI(LineNo, input/2);
   total_gate.insert(pair<unsigned, CirGate*>(input/2, newG));
   pilist.push_back(newG);
   return 1;
}

bool
CirMgr::readPO(const string& str, const int& LineNo, const int& id)
{
   int output;
   if(!myStr2Int(str, output)) {
      parseError(ILLEGAL_NUM);
      return 0;
   }
   // if(output >= 2*M+1) {
   //    parseError(MAX_LIT_ID);
   //    return 0;
   // }
   _outList.push_back(id);
   CirGate* newG = new PO(LineNo, id);
   newG->todolist.push(output);
   total_gate.insert(pair<unsigned, CirGate*>(id, newG));
   polist.push_back(newG);
   return 1;
}

// return 0 for format error, return 1 for end of gatedef, return 2 for continue

int
CirMgr::readGate(const string& str, const int& LineNo)
{
   if(str.empty()) return 0;
   if(str == "c") return 1; // start of comment
   if(str[0]-'0' > 9 || str[0]-'0' < 0) {
      parseError(EXTRA_SPACE);
      return 0;
   }
   int sizeoftok = 0;
   int* tok = new int[3]; // gate, input1, input2
   sizeoftok = parsegate(str, tok);
   if(sizeoftok == 0)   return 1; // end of gatedef
   else if(sizeoftok == -1) return 0;
   else if(sizeoftok != 3) { // format error
      parseError(MISSING_SPACE);
      return 0;
   }   
   ///
   // if(find(_inList.begin(), _inList.end(), tok[0]/2) != _inList.end()) {
   //    parseError(REDEF_CONST);
   //    return 0;
   // }
   // if(find(_gateList.begin(), _gateList.end(), tok[0]/2) != _gateList.end()){
   //    parseError(REDEF_CONST);
   //    return 0;
   // }
   ///
   if(tok[0] == 0 || tok[0] == 1) {
      parseError(REDEF_CONST);
      return 0;
   }
   CirGate* newG = new AigGate(LineNo, tok[0]/2);
   glist.push_back(newG);
   _gateList.push_back(tok[0]/2);
   total_gate.insert(pair<unsigned, CirGate*>(tok[0]/2, newG));
   for(int i = 1; i <= 2; ++i) {
      newG->todolist.push(tok[i]);
   }
   
   return 2;
}

bool
CirMgr::readName(const string& str)
{
   size_t pos = str.find_first_of(' '); // pos of space
   if(pos == string::npos)   return 0;
   string def = str.substr(0, pos);
   if(str.find_first_not_of(' ', pos) == string::npos)   return 0; // only one token
   string name = str.substr(pos+1);
   if(def[0] == 'i' || def[0] == 'o') {
      string index = def.substr(1);
      int id;
      if(!myStr2Int(index, id))  return 0; 
      if (def[0] == 'i') {
         if(id >= _inList.size())   return 0;
         map<unsigned, CirGate*>::iterator it = total_gate.find(_inList[id]);
         it->second->name = name;
      }
      else {
         if(id >= _outList.size())  return 0;
         map<unsigned, CirGate*>::iterator it = total_gate.find(_outList[id]);
         it->second->name = name;
      }
   }
   else return 0;
}

bool 
CirMgr::parsedef(const string& str)
{
   //size_t begin = 0;
   int* miloa = new int[5];
   size_t ex_space = str.find_first_of(' ');
   size_t begin    = str.find_first_not_of(' ');
   size_t wspace   = 0;
   if(ex_space == 0) { // extra space before aag def
      colNo = ex_space;
      parseError(EXTRA_SPACE);
      return 0;
   }
   if(int(str[wspace]) < 32) { // extra wspace between aag and M
      colNo = wspace;
      errInt = int(str[wspace]);
      parseError(ILLEGAL_WSPACE);
      return 0;
   }
   size_t end = str.find_first_of(' ', begin);
   string tmp = str.substr(begin, end - begin);
   if(tmp != "aag")  { // first def not aag
      if(tmp.size() >= 3 && tmp[3]-'0' <= 9) { // no space between aag and number
         colNo = 3;
         parseError(MISSING_SPACE);
         return 0;
      }
      errMsg = tmp;
      parseError(ILLEGAL_IDENTIFIER);
      return 0;
   }
   ex_space = str.find_first_of(' ', end+1);
   if(ex_space == end+1) { // extra space between aag and M
      colNo = ex_space;
      parseError(EXTRA_SPACE);
      return 0;
   }
   wspace = end+1;

   if(str[wspace] == '\t') { // extra wspace between aag and M
      colNo = wspace;
      errInt = int(str[wspace]);
      parseError(ILLEGAL_WSPACE);
      return 0;
   }
   if(end == string::npos) { // only aag
      colNo = str.find_last_not_of(' ')+1;
      errMsg = "number of variables";
      parseError(MISSING_NUM);
      return 0;
   }

   for(int i = 0; i < 5; ++i){ // get MILOA
      begin = str.find_first_not_of(' ', end);
      ex_space = str.find_first_of(' ', end+1);
      if(ex_space == end+1) { // extra space between def
         colNo = ex_space;
         parseError(EXTRA_SPACE);
         return 0;
      }
      if(i != 4) {
         wspace = GetNotNum(str, begin);
         if(wspace != 0) { // missing space between def
            colNo = wspace;
            errInt = int(str[wspace]);
            parseError(MISSING_SPACE);
            return 0;
         }
         wspace = str.find_first_of(' ', begin)+1;
         if(int(str[wspace]) < 32) { // extra wspace
            colNo = wspace;
            errInt = int(str[wspace]);
            parseError(ILLEGAL_WSPACE);
            return 0;
         }
      }

      colNo = str.find_last_of(str.back())+1;
      if(begin == string::npos) { // no def 
         switch(i) {
            case 0:
               errMsg = "number of variables";
               parseError(MISSING_NUM);
               return 0;
            case 1:
               errMsg = "number of PIs";
               parseError(MISSING_NUM);
               return 0;
            case 2:
               errMsg = "number of latches";
               parseError(MISSING_NUM);
               return 0;
            case 3:
               errMsg = "number of POs";
               parseError(MISSING_NUM);
               return 0;
            case 4:
               errMsg = "number of AIGs";
               parseError(MISSING_NUM);
               return 0;
         }
         return 0; 
      }
      end = str.find_first_of(' ', begin);
      tmp = str.substr(begin, end-begin);
      if(!myStr2Int(tmp, miloa[i]) || miloa[i] < 0) { // illegal number
         switch(i) { 
            case 0:
               errMsg = "number of variables(";
               errMsg.append(tmp);
               errMsg.append(")");
               parseError(ILLEGAL_NUM);
               return 0;
            case 1:
               errMsg = "number of PIs(";
               errMsg.append(tmp);
               errMsg.append(")");
               parseError(ILLEGAL_NUM);
               return 0;
            case 2:
               errMsg = "number of latches(";
               errMsg.append(tmp);
               errMsg.append(")");
               parseError(ILLEGAL_NUM);
               return 0;
            case 3:
               errMsg = "number of POs(";
               errMsg.append(tmp);
               errMsg.append(")");
               parseError(ILLEGAL_NUM);
               return 0;
            case 4:
               errMsg = "number of AIGs(";
               errMsg.append(tmp);
               errMsg.append(")");
               parseError(ILLEGAL_NUM);
               return 0;
               
         }
      };
      if(i == 4) {
         if(str.back() == ' ' || (str.back()-'0' > 9 && str.back()-'0' < 0)) { // no newline after aag def
            colNo = end;
            parseError(MISSING_NEWLINE);
            return 0;
         }
      }
   }
   
   
   M = miloa[0]; I = miloa[1]; L = miloa[2]; O = miloa[3]; A = miloa[4];
   if(M < L+I+A) {  // M != L+I+A
      errInt = M;
      errMsg = "Number of variables";
      parseError(NUM_TOO_SMALL);
      return 0;
   }
   else if(L > 0) {
      errMsg = "latches";
      parseError(ILLEGAL_NUM);
      return 0;
   }
   return 1;

} 

size_t
CirMgr::GetNotNum(const string& str, size_t pos = 0)
{
   for(size_t i = pos; i < str.size(); ++i) {
      if(str[i]-'0' < 9 && str[i]-'0' > 0) continue;
      else if(str[i] == ' ')  return 0;
      else if(int(str[i]) < 32)   return i;
   }
   return 0;
}

int 
CirMgr::parsegate(const string& str, int* id)
{
   size_t pos = 0;
   string tok;
   string _str = str;
   int size = 0;
   ///
   if(str.find_first_of(' ', str.find_first_of(' ', str.find_first_of(' ')+1)+1) != string::npos) {
      parseError(EXTRA_SPACE);
      return -1;
   }
   if(str[0] == ' ') {
      parseError(EXTRA_SPACE);
      return -1;
   }
   ///
   if(_str[_str.size()] == ' ') {
      parseError(EXTRA_SPACE);
      return -1;
   }
   else _str.append(" ");
   while(true) {
      pos = myStrGetTok(_str, tok, pos);
      if(pos == string::npos) break;
      if(size >= 3) {
         parseError(MISSING_NEWLINE);
         return -1; // 有 >=4個token
      }
      if(!myStr2Int(tok, id[size])) return 0; // format error or end of gatedef
      size++;
   }
   return size;
}
/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI " << setw(11) << right <<  _inList.size()   << endl;
   cout << "  PO " << setw(11) << right << _outList.size()  << endl;
   cout << "  AIG" << setw(11) << right <<_gateList.size() << endl; 
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << _inList.size()+_outList.size()+_gateList.size() << endl;
}

void
CirMgr::printNetlist() const
{
   Globalref++;
   cout << endl;
   for(size_t i = 0; i < polist.size(); ++i){
      polist[i]->dfsnet(lineNo);
   }
   lineNo = 0;
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i = 0; i < _inList.size(); ++i){
      cout << " ";
      cout << _inList[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i = 0; i < _outList.size(); ++i){
      cout << " ";
      cout << _outList[i];
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const // in id order, not define order
{
   vector<int> fgate;
   vector<int> ugate;
   for(size_t i = 0; i < glist.size(); ++i){
      if(glist[i]->floating)   fgate.push_back(glist[i]->id);
   }
   for(size_t i = 0; i < polist.size(); ++i){
      if(polist[i]->_faninList[0].gate()->getTypeStr() == "UNDEF")  fgate.push_back(polist[i]->id);
   }
   for(size_t i = 0; i < pilist.size(); ++i) {
      if(pilist[i]->_fanoutList.empty())  ugate.push_back(pilist[i]->id);
   }
   for(size_t i = 0; i < glist.size(); ++i){
      if(glist[i]->_fanoutList.empty())   ugate.push_back(glist[i]->id);
   }
   sort(fgate.begin(), fgate.end());
   sort(ugate.begin(), ugate.end());
   if(fgate.size() != 0)   {
      cout << "Gates with floating fanin(s): ";
      for(size_t i = 0; i < fgate.size(); ++i){
         cout << fgate[i];
         if(i != fgate.size()-1) cout << " ";
      }
      cout << endl;
   }
   if(ugate.size() != 0){
      cout << "Gates defined but not used  : ";
      for(size_t i = 0; i < ugate.size(); ++i){
         cout << ugate[i];
         if(i != ugate.size()-1) cout << " ";
      }
      cout << endl;
   }   
}

void
CirMgr::writeAag(ostream& outfile) const
{
   ++Globalref;
   int DefAigNum = 0;
   vector<string> toPrint;
   for(size_t i = 0; i < polist.size(); i++){
      dfs(polist[i], toPrint, DefAigNum);
   }

   outfile << "aag " << M << " " << I << " " << L << " " << O << " " << DefAigNum << endl;
   for(int i = 2; i < I+O+2; ++i) {
      outfile << content[i] << endl;
   }
   for(size_t i = 0; i < toPrint.size(); ++i) {
      outfile << toPrint[i] << endl;
   }
   for(size_t i = 0; i < pilist.size(); ++i) {
      if(!pilist[i]->name.empty()) {
         outfile << "i" << i << " ";
         outfile << pilist[i]->name  << endl;
      }
   }
   for(size_t i = 0; i < polist.size(); ++i) {
      if(!polist[i]->name.empty()) {
         outfile << "o" << i << " ";
         outfile << polist[i]->name << endl;
      } 
   }
   outfile << "c" << endl;
   // for(size_t i = 0; i < content.size(); ++i){
   //    IdList::const_iterator it = find(idp.begin(), idp.end(), unsigned(i));
   //    if(it != idp.end())  continue;
   //    outfile << content[i] << endl;
   // }
   outfile << "AAG output by Chun-Ting Chen, Merry Christmas!!" << endl;
}

void
CirMgr::dfs(CirGate* g, vector<string>& toPrint, int& n) const
{
   if(g->ref == Globalref) return;
   g->setGlobalref();
   for(size_t i = 0; i < g->_faninList.size(); ++i) {
      dfs(g->_faninList[i].gate(), toPrint, n);
   }
   if(g->getTypeStr() == "AIG") { 
      ++n; 
      string s;
      s.append(to_string(2*g->id));
      s.append(" ");
      if(g->_faninList[0].isInv())  s.append(to_string(2*g->_faninList[0].gate()->id+1));
      else s.append(to_string(2*g->_faninList[0].gate()->id));
      s.append(" ");
      if(g->_faninList[1].isInv())  s.append(to_string(2*g->_faninList[1].gate()->id+1));
      else s.append(to_string(2*g->_faninList[1].gate()->id));
      toPrint.push_back(s);
   }

}