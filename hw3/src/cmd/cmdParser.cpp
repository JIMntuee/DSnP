/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

#define USAGE_TAG 1000
using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO:...
   // return false;
   if(_dofileStack.size() >= 1024){
      cerr << "Error: Stack's depth has reached 1024!!";
      return false;
   }
   _dofile = new ifstream(dof.c_str());
   if(!_dofile->is_open()){
      if(!_dofileStack.empty())  _dofile = _dofileStack.top();
      return false;
   }
   else   _dofileStack.push(_dofile);
   return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   // TODO:...
   delete _dofile;
   _dofileStack.pop();
   if(!_dofileStack.empty()){
      _dofile = _dofileStack.top();
   }
   else _dofile = 0;
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);//read *_dofile if it hasn't done
   else
      newCmd = readCmd(cin);//read user cin after execute *_dofile

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO:... 
   CmdMap::const_iterator  start = _cmdMap.begin();
   CmdMap::const_iterator  end = _cmdMap.end();
   for (CmdMap::const_iterator it = start; it != end; it++){
      it->second->help();
   } 
   cout << endl;


}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"

//option 要紀錄後面的參數
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO:...
   assert(str[0] != 0 && str[0] != ' ');
   string cmd = "";// first word in str
   CmdExec* command;
   size_t idx = myStrGetTok(str, cmd);
   if(idx !=  string::npos)  option.append(str.begin()+idx+1, str.end());
   else option = "";
   //myStrGetTok(str, option, myStrGetTok(str, cmd));
   command = getCmd(cmd);
   if(!command){
      cerr << "Illegal command!! " << str;
      return 0;
   }
   else return command;
}


// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
   // TODO:...
   vector<string> option;
   size_t idx = 0;
   while (true){//parse option
      string temp;
      idx = myStrGetTok(str, temp, idx);
      if(!temp.size())  break;
      option.push_back(temp);
   }  

   if(!option.size()){//1.....nothing but ' ' was input
      printallcommand(str);
      _tabPressCount = 0;
      return ;
   }

   //rest.....
   else if(option.size() == 1){//option has only one word
      vector<CmdMap::iterator> pos_exe;//matched commands
      for(CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){//finding match command
         string command = it->first;
         command.append(it->second->getOptCmd());
         if(option[0].size() > command.size())  continue;   
         else if(!(myStrNCmp(command, option[0], option[0].size()))){
            pos_exe.push_back(it);
         }
      }
      if(!pos_exe.size()){
         mybeep();//4.....no match
         return ;
      }  
      else if(pos_exe.size() == 1){ //3&5&6.....only one command match
         if(_readBuf[_readBufPtr-_readBuf - 1] == ' '){//print filename or usage
            if(_tabPressCount >= 2){//print filename
               vector<string> file_names;
               string prefix = "";
               listDir(file_names, prefix, ".");
               if(!file_names.size()){
                  mybeep();
                  return;
               }
               else if(file_names.size() == 1){
                  for(size_t i = 0; i < file_names[0].size(); i++){
                     insertChar(file_names[0][i]);
                  }
                  return;
               }
               else{
                  string pre = getcomprefix(file_names);
                  if(pre == ""){//no common prefix
                     for(size_t i = 0; i < file_names.size(); i++){
                        if(!(i%5))  cout << endl;
                        cout << setw(16) << left << file_names[i];
                     }
                     reprintCmd();
                     return;
                  }
                  else{
                     for(size_t i = 0; i < pre.size(); i++){
                        insertChar(pre[i]);
                     }
                     mybeep();
                     return;
                  }
               }
            }
            else{//print usage
               cout << endl;
               pos_exe[0]->second->usage(cout);
               reprintCmd();
               _tabPressCount = 1;
               return;
            }
         }
         else{
            /////////print rest command character
            string command = pos_exe[0]-> first;
            for(size_t i = 0; i < command.size(); i++){
               command[i] = tolower(command[i]);
            }
            command.append(pos_exe[0]->second->getOptCmd());
            command = command.substr(option[0].size());
            for(size_t i = 0; i < command.size(); i++){
               insertChar(command[i]);
            }
            insertChar(' ');
            _tabPressCount = 0;
            return;
         }
      }
      else{//multiple command match
         for(size_t i = 0; i < pos_exe.size(); i++){
            string command = pos_exe[i]->first;
            command.append(pos_exe[i]->second->getOptCmd());
            if(!(i%5))  cout << endl;
            cout << setw(12) << left << command;
         }
         reprintCmd();
         _tabPressCount = 0;
         return;
      }

      
   }
   else{//multiple option
      CmdMap::iterator exe;
      bool match = false;
      for(CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){//finding match command
         string command = it->first;
         command.append(it->second->getOptCmd());
         if(option[0].size() > command.size())  continue;   
         else if(!(myStrNCmp(command, option[0], option[0].size()))){
            exe = it;
            match = true;
         }
      }
      if(!match){
         mybeep();//7.....no match
         _tabPressCount = 0;
         return ;
      }
      else{
         if(_tabPressCount >= 2){
            vector<string> file_names;
            string prefix = option.back();
            listDir(file_names, prefix, ".");
            if(!file_names.size())  return;
            else if(file_names.size() == 1){//only one filename match
               if(_readBuf[_readBufPtr-_readBuf-1] == ' ')  return;
               if(file_names[0].size() == prefix.size()){
                  insertChar(' ');
                  return;
               }
               string insert = file_names[0].substr(prefix.size());
               for(size_t i = 0; i < insert.size(); i++){
                  insertChar(insert[i]);
               }
               insertChar(' ');
               return;
            }
            else{//mutiple filename match
               string file_prefix;
               file_prefix = getcomprefix(file_names);
               if(file_prefix == prefix){
                 for(size_t i = 0; i < file_names.size(); i++){
                     if(!(i%5))  cout << endl;
                     cout << setw(16) << left << file_names[i];
                  }
                  reprintCmd();
                  return; 
               }
               else{
                  for(size_t i = 0; i < prefix.size(); i++){
                     moveBufPtr(_readBufEnd-1);
                     deleteChar();
                  }
                  for(size_t i = 0; i < file_prefix.size(); i++){
                     insertChar(file_prefix[i]);
                  }
                  return;
               }
            }
         }
         else{//print usage
            cout << endl;
            exe->second->usage(cout);
            reprintCmd();
            _tabPressCount = 1;
            return;
         }
      }
   }
   
}

void
CmdParser::printallcommand(const string& str){
   int countcom = 0;
      for(CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); it++){
         string command = it->first;
         command.append(it->second->getOptCmd());
         if(!(countcom%5)) cout << endl;
         cout << setw(12) << left << command;
         countcom++;
      }
      cout << endl;
      printPrompt();
      for(size_t i = 0; i < str.size(); i++){
         insertChar(str[i]);
      }
}

void
CmdParser::findcomprefix(const vector<string>& files, vector<string>& matched,  const string& prefix){
   for(size_t i = 0; i < files.size(); i++){
      if(!myStrNCmp(files[i], prefix, prefix.size())){
         matched.push_back(files[i]);
      }
   }
}

string
CmdParser::getcomprefix(const vector<string>& files){
   string temp = files[0];
   string common = "";
   for(size_t i = 0; i < files[0].size(); i++){
      for(size_t j = 1; j < files.size(); j++){
         if(files[j][i] != temp[i])   return common;
         if(j == files.size()-1)  common.append(1, temp[i]);
      }
   }
   return common;
}
// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO:...
   CmdMap::iterator  start = _cmdMap.begin();
   CmdMap::iterator  end = _cmdMap.end();
   if(start == end)  return e;
   for(CmdMap::iterator  it = start; it != end; it++){
      string command = it->first;
      command.append(it->second->getOptCmd());
      int result = myStrNCmp(command, cmd, it->first.size());
      if(result == 0){
         e = it->second;
         return e;
      } 
   }
   
   return 0;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token

// 查option 中是否有東西
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) { //err != 0 表示有option
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true

// optional 表示是否一定要有option
// if (!optional && no token) || more than one option  return false
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const

{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.

//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

