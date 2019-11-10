/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string content;
   while(getline(is, content)){
      string tempstr;
      int tempval;
      if(j.get_key(content) == "0")  continue;
      tempstr = j.get_key(content);
      tempval = j.get_value(content);
      if(tempstr != "0"){
         DBJsonElem temp(tempstr, tempval);
         j._obj.push_back(temp);
      }
   }
   is.clear();
   
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO:
   os << "{" << endl;
   for(size_t i = 0; i < j.size(); i++){
      os <<"  ";
      os << j._obj[i];
      if(i != j.size() - 1)   os << ",";
      os << endl;
   }
   os << "}";
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO:
   read_in = false;
   size_t tmp_size = size();
   for(size_t i = 0; i < tmp_size; i ++){
      _obj.pop_back();
   }
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO:
   for(size_t i = 0; i < size(); i++){
      if(_obj[i].key() == elm.key())   return false;   
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO:
   float total = 0;
   if(size() == 0)  return NAN;
   for(size_t i = 0; i < size(); i++){
      total += _obj[i].value();
   }
   return (total/float(size()));
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO:
   if(size() == 0){  idx = size();  return INT_MIN;}
   int maxN = INT_MIN;
   for(size_t i = 0; i < size(); i++){
      if(_obj[i].value() > maxN){ 
         maxN = _obj[i].value();
         idx = i;
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO:
   if(size() == 0){  idx = size();  return INT_MAX;}
   int minN = INT_MAX;
   for(size_t i = 0; i < size(); i++){
      if(_obj[i].value() < minN){
         minN = _obj[i].value();
         idx = i;
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO:
   if(size() == 0)   return 0;
   int s = 0;
   for(size_t i = 0; i < size(); i++){
      s += _obj[i].value();
   }
   return s;
}

string
DBJson::get_key(const string& str){
   int index1, index2;
   string key;
   if(str.find('"') == string::npos)   return "0";
   index1 = str.find('"')+1;
   index2 = str.find('"', index1+1)-1;
   key.append(str, index1, index2-index1+1);
   return(key);
}

int
DBJson::get_value(const string& str){
   int index1 = 0 , index2 = 0;
   int value = 0;
   bool negative = false;
   string val;
   index1 = str.find(':');
   for(size_t i = str.find(':'); i < str.size(); i++){
      if(str[i] == 45){
         negative = true;
      }
      if(str[i] >= 48 && str[i] <= 57){
         index1 = i;
         break;
      } 
   } //find pos of first char of value
   for(size_t i = index1; i <= str.size(); i++){
      if(!(str[i] >= 48 && str[i] <= 57)){
         index2 = i - 1;
         break;
      } 
   } //find pos of last char of value
   if(index2 == 0) index2 = str.size()-1;
   val.append(str, index1, index2 - index1 + 1);
   value = stoi(val);
   if(negative)   return -value;
   else  return value;
}