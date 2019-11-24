#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>

#define add_size 1000000
#define query_size 1000000
#define delete_size 10000
#define string_size 5

using namespace std;

string genstring(int size){
    string _str;
    _str.resize(string_size);
    for (int i = 0; i < string_size; ++i)
      _str[i] = 'a' + (rand()%26);
    return _str;
}

int main(){
    srand(time(NULL));
    vector<string>  data;
    for(int i = 0; i < add_size; i++){
        string d = genstring(string_size);
        data.push_back(d);
        cout << "adta -s " << d << endl;
    }
    // cout << "adtr " << string_size << endl;
    // cout << "adta -r " << add_size << endl;
    cout << "usage" << endl;

    random_shuffle(data.begin(), data.end());
    for(size_t i = 0; i < query_size; i++ ){
        cout << "adtq " << data[i] << endl;
    }
    // cout << "adtd -r " << delete_size << endl;
    // cout << "adts" << endl;
    cout << "usage" << endl;
    cout << "q -f" << endl;
}