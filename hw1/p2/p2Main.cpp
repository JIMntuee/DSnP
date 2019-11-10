/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;
   string command;
   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   cout << "Enter command: ";

   while (true) {
      cin >> command;
      if(command == "EXIT") return 0;
      else if(command == "ADD")  json.add();
      else if(command == "PRINT")  json.print();
      else if(json.check()){
            cout << "Error: No element found!!" << endl;
            cout << "Enter command: ";
            continue;
      }
      else if(command == "SUM")  json.sum();
      else if(command == "AVE")  json.avg();
      else if(command == "MAX")  json.max();
      else if(command == "MIN")  json.min();
      cout << "Enter command: ";
   }
}
