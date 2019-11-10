/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <iomanip>

using namespace std;

// Implement member functions of class Row and Table here
bool Json::read(const string& jsonFile)
{
   string content;
   fstream infile;
   infile.open(jsonFile);
   if(!infile) return 0;
   

   while(getline(infile, content)){
      string tempstr;
      int tempval;
      //if(content == "}")   break;
      //if(content == "{")   continue;
      if(get_name(content) == "0")  continue;
      tempstr = get_name(content);
      tempval = get_value(content);
      if(tempstr != "0"){
         JsonElem temp(tempstr, tempval);
         _obj.push_back(temp);
      }
      //cout << content << endl;
   }
   
   return true; // TODO
}

string Json::get_name(string str){
   //cout << "test";
   int index1, index2;
   string name;
   // if(str.size() == 0)  return "0";
   // for(int i = 0; i < str.size(); i++){
   //    if(str[i] == '"') break;
   //    else if(i == (str.size() - 1))  return "0";
   // }
   if(str.find('"') == string::npos)   return "0";
   index1 = str.find('"')+1;
   index2 = str.find('"', index1+1)-1;
   //if(index1 == 0 || index2 == 0) return 0;
   name.append(str, index1, index2-index1+1);
   return(name);
}

int Json::get_value(string str){
   int index1 = 0 , index2 = 0;
   int value = 0;
   bool negative = false;
   string val;
   index1 = str.find(':');
   for(int i = str.find(':'); i < str.size(); i++){
      if(str[i] == 45){
         negative = true;
      }
      if(str[i] >= 48 && str[i] <= 57){
         index1 = i;
         break;
      } 
   } //find pos of first char of value
   for(int i = index1; i <= str.size(); i++){
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

bool Json::check()
{
   if(_obj.size() == 0) return 1;
   else return 0;
}

void Json::print() 
{
   cout << "{" << endl;
   for(int i = 0 ; i < _obj.size(); i++){
   cout << "  " << _obj[i];
   i == _obj.size() - 1 ? cout << endl : cout << "," << endl;   
   }
   cout << "}" << endl;
}

ostream& operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void Json::sum()
{
   int sum = 0;
   for(int i = 0 ; i < _obj.size(); i++){
      sum += _obj[i].readvalue();
   }
   cout << "The summation of the values is: " << sum << "." << endl;
}

void Json::avg()
{
   double _sum;
   double _avg;
   for(int i = 0 ; i < _obj.size(); i++){
      _sum += _obj[i].readvalue();
   }
   _avg = _sum / _obj.size();
   cout << "The average of the values is: " << fixed << setprecision(1) << _avg << "." << endl;
}

void Json::max()
{
   int max = _obj[0].readvalue();
   int maxindex = 0;
   for(int i = 1 ; i < _obj.size() ; i++){
      if(_obj[i].readvalue() > max){
         max = _obj[i].readvalue();
         maxindex = i;
      } 
   }
   cout << "The maximum element is: { " << _obj[maxindex] << " }." << endl;
}

void Json::min()
{
   int min = _obj[0].readvalue();
   int minindex = 0;
   for(int i = 1 ; i < _obj.size(); i++){
      if(_obj[i].readvalue() < min){
         min = _obj[i].readvalue();
         minindex = i;
      } 
   }
   cout << "The minimum element is: { " << _obj[minindex] << " }." << endl;
}

void Json::add()
{
   string name, value;
   int val;
   bool negative = false;
   cin >> name >> value;
   if (value[0] == 45){
      negative == true;
   }
   for(int i = 0; i < value.size(); i++){
      if((value[i] >= 58 || value[i] <= 47 ) && value[i] != 45){
         cout << "Illegal Data Type!" << endl;
         return; 
      }
   }
   if(!negative){  
      val = stoi(value);
      JsonElem elem(name, val);
      _obj.push_back(elem);
   }
   else {
      value.erase(0,1);
      val = stoi(value);
      JsonElem elem1(name, -val);
      _obj.push_back(elem1);
   }
}