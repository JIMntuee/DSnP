/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) { }
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n = 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }//FIXME:
      T& operator * () { return (*_node); }
      iterator& operator ++ () {
         _node++;
         return (*this); 
      }
      iterator operator ++ (int) {
         Array::iterator tmp = *this; 
         _node++;
         return (tmp); 
      }
      iterator& operator -- () { 
         _node--;
         return (*this);
      }
      iterator operator -- (int) { 
         Array::iterator tmp = *this;
         _node--;
         return (tmp); 
      }

      iterator operator + (int i) const {
         Array::iterator tmp = *this;
         tmp._node += i;         
         return (tmp); 
      }
      iterator& operator += (int i) { 
         _node += i;
         return (*this); 
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return (*this); 
      }

      bool operator != (const iterator& i) const { 
         if(_node != i._node) return true;
         else  return false; 
      }
      bool operator == (const iterator& i) const { 
         if(_node == i._node) return true;
         else  return false; 
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if(_capacity == 0)   return 0;
      else {
         return iterator(_data);
      }
   }
   iterator end() const { 
      if(_capacity == 0)   return 0;
      else {
         return iterator(_data+_size);
      } 
   }
   bool empty() const { return !_size; }
   size_t size() const { return _size; }
   
   T& operator [] (size_t i) { 
      if(i >= _size){
         cerr << "Error: \"" << i << "\" is not a legal index !!";
         return 0;
      }
      return *(begin()+i);
   }
   const T& operator [] (size_t i) const { 
      if(i >= _size){
         cerr << "Error: \"" << i << "\" is not a legal index !!";
         return 0;
      }
      return *(begin()+i);
   }

   void push_back(const T& x) {
      if(_size == _capacity)  expand();
      _data[_size++] = x;
   }
   void pop_front() { 
      if(empty()) return;
      _data[0] = _data[--_size];
   }
   void pop_back() { 
      if(empty()) return;  
      --_size;
   }
   void expand() { 
      size_t cap = 0;
      if ( _capacity == 0 ) cap = 1;
      else cap = _capacity*2;
      T* new_data = new T[cap];
      for(size_t i = 0; i < _size; i++){
         new_data[i] = _data[i];
      }
      delete []_data;
      _capacity = cap;
      _data = new_data;
   }
   
   bool erase(iterator pos) {
      if(empty()) return false;
      *(pos._node) = _data[--_size];
      return true;
   }
   bool erase(const T& x) { 
      iterator pos = find(x);
      if(pos == end())  return false;
      erase(pos);
      return true;
   }
   iterator find(const T& x) {
      iterator pos = begin();
      for(; pos != end(); pos++){
         if(*pos == x)   break;
      }
      return pos;
   }

   void clear() { _size = 0; }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size);}

   // Nice to have, but not required in this homework...
   // reserve at least n size of array

   void reserve(size_t n) { 
      while(_capacity < n){
         expand();
      }
   }

   // resize to n
   // if n > _size, fill the rest with x

   void resize(size_t n, const T& x = T("0")) { 
      if(n < _size){ _size = n; return; }
      else{
         reserve(n);
         size_t org_size = _size;
         _size += n;
         for(size_t i = org_size; i < _size; i++){
            _data[i] = x;
         }
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
