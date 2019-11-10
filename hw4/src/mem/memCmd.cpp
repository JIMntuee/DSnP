/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO:
   vector<string> tokens;
   lexOptions(option, tokens, 0);
   if(tokens.size() == 0){
      return errorOption(CMD_OPT_MISSING, "");
   }
   int times = 0;
   if(tokens.size() == 1){
      if(!myStr2Int(tokens[0], times) || times <= 0){
         return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
      }
      mtest.newObjs(times);
   }
   else if(tokens.size() == 3){
      int index = 0;
      int pos;
      if(!myStrNCmp("-array", tokens[0], 2)) pos = 0;
      else if(!myStrNCmp("-array", tokens[1], 2))  pos = 1;
      else return errorOption(CMD_OPT_ILLEGAL, "");
      if(pos == 0){
         if(!myStr2Int(tokens[2], times) || times <= 0){
            return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
         }
         else if(!myStr2Int(tokens[1], index) || index <= 0){
            return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
      }
      else if(pos == 1){
         if(!myStr2Int(tokens[0], times) || times <= 0){
            return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
         }
         else if(!myStr2Int(tokens[2], index) || index <= 0){
            return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
         }
      }
      try{
         mtest.newArrs(times, index);
      }  catch(bad_alloc e){
         return CMD_EXEC_ERROR;
      }
      
   }
   else  return errorOption(CMD_OPT_EXTRA, "");
   
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO:
   vector<string> tokens;
   size_t objlsize = mtest.getObjListSize();
   size_t arrlsize = mtest.getArrListSize();
   lexOptions(option, tokens, 0);
   if(tokens.size() == 0 || tokens.size() == 1){
      return errorOption(CMD_OPT_MISSING, "");
   }
   if(tokens.size() == 2){
      int num;
      if(!myStr2Int(tokens[1], num) || num <= 0)   errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      if(!myStrNCmp("-index", tokens[0], 2)){
         if(num >= int(objlsize)){
            cerr << "Size of object list (" << objlsize << ") is <= " << num << "!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
         mtest.deleteObj(num);
      }
      else if(!myStrNCmp("-random", tokens[0], 2)){
         if(!objlsize){
            cerr << "Size of object list is 0!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
         }
         for(int i = 0; i < num; i++){
            mtest.deleteObj(rnGen(objlsize));
         }
      }
      else return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }

   else if(tokens.size() == 3){
      int num = 0;
      if(!myStrNCmp("-array", tokens[0], 2)){
         if(!myStrNCmp("-index", tokens[1], 2)){
            if(!myStr2Int(tokens[2], num) || num < 0)   return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
            if(num >= int(arrlsize)){
               cerr << "Size of array list (" << arrlsize << ") is <= " << num << "!!";
               return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
            }
            mtest.deleteArr(num);
         }
         else if(!myStrNCmp("-random", tokens[1], 2)){
            if(!myStr2Int(tokens[2], num) || num <= 0)   return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
            if(!arrlsize){
               cerr << "Size of array list (" << arrlsize << ") is <= " << num << "!!";
               return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
            }
            for(int i = 0; i < num; i++){
               mtest.deleteArr(rnGen(arrlsize));
            }
         }
         else{
            return CMD_EXEC_ERROR;
         }
      }
      else if(!myStrNCmp("-array", tokens[2], 2)){
         if(!myStrNCmp("-index", tokens[0], 2)){
            if(!myStr2Int(tokens[1], num) || num < 0)   return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
            if(num >= int(arrlsize)){
               cerr << "Size of array list (" << arrlsize << ") is <= " << num << "!!" << endl;
               return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
            }
            mtest.deleteArr(num);
         }
         else if(!myStrNCmp("-random", tokens[0], 2)){
            if(!myStr2Int(tokens[1], num) || num <= 0)   return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
            if(!arrlsize){
               cerr << "Size of array list (" << arrlsize << ") is <= " << num << "!!" << endl;
               return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
            }
            for(int i = 0; i < num; i++){
               mtest.deleteArr(rnGen(arrlsize));
            }
         }
         else{
            return CMD_EXEC_ERROR;
         }
      }
      else return CMD_EXEC_ERROR;
   }
   else{
      return errorOption(CMD_OPT_EXTRA, "");
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


