/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO:
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
      if(_data.size() == 0) {
         _data.push_back(d);
         return;
      }
      int t = _data.size();
      int p = (t+1)/2-1;
      if(_data[p] < d || _data[p] == d) _data.push_back(d);
      else {
         _data.push_back(_data[p]);
         t = p;
         while(t > 0) {
            p = (t+1)/2-1;
            if(_data[p] < d || _data[p] == d) break;
            _data[t] = _data[p];
            t = p;
         }
         _data[t] = d;
      }
   }
   void delMin() { 
      int n = _data.size()-1;
      int p = 0, t = 2*(p+1)-1; // p = parent, t = left children
      while(t <= n) {
         if(t < n) { // has right sibling
            if(_data[t+1] < _data[t] || _data[t+1] == _data[t]) ++t;
         } 
         if(_data[n] < _data[t] || _data[n] == _data[t]) break;
         _data[p] = _data[t];
         p = t;
         t = 2*(p+1)-1;
      }
      _data[p] = _data[n];
      _data.pop_back();
   }
   void delData(size_t i) { 
      int n = _data.size()-1;
      int p = i, t = 2*(p+1)-1; // p = parent, t = left children
      while(t <= n) {
         if(t < n) { // has right sibling
            if(_data[t] < _data[t+1]) ++t;
         } 
         if(_data[n] < _data[t] || _data[n] == _data[t]) break;
         _data[p] = _data[t];
         p = t;
         t = 2*(p+1)-1;
      }
      _data[p] = _data[n];
      _data.pop_back();
   }
   void rearrange() {
      if(_data.size() == 1)   return;
      else if(_data.size() == 2) {
         if(_data[1] < _data[0]) {
            Data n = _data[0];
            delMin();
            insert(n);
         }
      }
      else if(_data.size() >= 3) {
         if(_data[1] < _data[0] || _data[2] < _data[0]) {
            Data n = _data[0];
            delMin();
            insert(n);
         }
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
