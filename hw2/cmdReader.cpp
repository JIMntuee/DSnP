/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : {
            if(_readBufPtr == _readBuf){
               mybeep();
               break;
            }
            else{
               moveBufPtr(_readBufPtr-1);
               deleteChar();
               break;
            }
         }
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : {
            /* TODO: */ 
            int ptr_position = _readBufPtr - _readBuf;
            insertChar(' ', 8 - (ptr_position%8));
            break;
         }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)//
{  
   int move = abs(_readBufPtr - ptr);
   if(ptr < _readBuf || _readBufEnd < ptr){
      mybeep();
      return false;
   }
   if(ptr < _readBufPtr){
      for(int i = 0; i < move; i++){
         cout << '\b';
         _readBufPtr--;
      }
   }
   else if(ptr > _readBufPtr){
      char* temp = _readBufPtr;
      for(int i = 0; i < move; i++){
         cout << temp[i];
         _readBufPtr++;
         //move the cursor to the right by one position
      }
   }
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO:...
   int end_position = _readBufEnd - _readBuf;
   int ptr_position = _readBufPtr - _readBuf;
   int mag = end_position - ptr_position ;//mag of string at right

   if(end_position == ptr_position){
      mybeep();
      return false;
   }

   for(int i = 0; i < mag; i++){
      if(i == (mag - 1)){
         _readBuf[ptr_position + i] = '\0';
      }
      else{
         _readBuf[ptr_position + i] = _readBuf[ptr_position + i + 1];
      }
   }
   for(int i = 0; i < mag; i++){
      if(i == (mag - 1))   cout << ' ';
      else  cout << _readBuf[ptr_position + i];
      _readBufPtr++;
   }

   moveBufPtr(_readBufPtr - mag);
   _readBufEnd--;

   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO:...
   assert(repeat >= 1);
   
   int end_position = _readBufEnd - _readBuf;
   int ptr_position = _readBufPtr - _readBuf;
   int mag =  end_position - ptr_position;//mag of string at right
   

   for(int i = 0; i < mag; i++){
      _readBuf[end_position + repeat -1 - i] = _readBuf[end_position - 1 - i];
   }
   for(int i = 0; i < repeat; i++){
      _readBuf[ptr_position + i] = ch;
   }
   for(int i = 0; i < mag + repeat; i++){
      cout << _readBuf[ptr_position + i];
      _readBufPtr++;
   }

   _readBufEnd += repeat;
   moveBufPtr(_readBufPtr - mag);
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO:...
   int end_position = _readBufEnd - _readBuf;
   moveBufPtr(_readBuf);
   for(int i = 0; i < end_position; i++){
      deleteChar();
   }
   _readBufEnd = _readBuf;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO:...
   if(index < _historyIdx){// check history upward
      if(_historyIdx == 0){
         mybeep();
         return;
      }
      if(_historyIdx == _history.size()){// add temp string
         string temp = _readBuf;
         _tempCmdStored = true;
         _history.push_back(temp);
      }
      else if(_tempCmdStored && _historyIdx == _history.size()-1){// repush the string
         string temp = _readBuf;
         _history.pop_back();
         _history.push_back(temp);
      }
      if(index < 0)  _historyIdx = 0;
      else  _historyIdx = index;
   }

   else if(index > _historyIdx){//check history downward
      if((_historyIdx == _history.size() || (_tempCmdStored && _historyIdx == _history.size()-1))){
         mybeep();
         return;
      }
      if(index >= _history.size()) _historyIdx = _history.size() - 1;
      else _historyIdx = index;
      // cout << _historyIdx;
   }
   retrieveHistory();
   
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.

// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()//
{
   // TODO:...
   //null string return
   if(_readBuf[0] == '\0'){
      return;
   }

   if(_tempCmdStored){//delete temp str
      _tempCmdStored = false;
      _history.pop_back();
      _historyIdx = _history.size();
   }

   string tmp = _readBuf;
   int count = 0;
   int str_size = tmp.size();

   //delete ' ' at the begining
   for(size_t i = 0; i < str_size; i++){
      if(tmp[i] != ' ') break;
      else if(i == str_size-1) return;//empty string
      else count++;
   }
   for(size_t i = 0; i < str_size-count; i++){
      tmp[i] = tmp[i+count];
   }
   tmp.resize(str_size-count);
   count = 0;
   str_size = tmp.size();

   //delete ' ' at the end
   for(size_t i = str_size-1; i > 0; i--){
      if(tmp[i] != ' ') break;
      else  count++;
   }
   tmp.resize(str_size-count);

   _history.push_back(tmp);

   size_t end_position = _readBufEnd - _readBuf;
   for(size_t i = 0; i < end_position; i++){
      _readBuf[i] = '\0';
   }

   _historyIdx = _history.size();

}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   if(_history[_historyIdx].empty()){
      moveBufPtr(_readBufPtr-1);
      _readBufEnd = _readBuf + _history[_historyIdx].size();
   }
   else{
      strcpy(_readBuf, _history[_historyIdx].c_str());
      cout << _readBuf;
      _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
   }
}
