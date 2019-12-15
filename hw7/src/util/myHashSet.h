/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
#include <algorithm>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(class vector<Data>::iterator it, const HashSet* h, size_t i): _it(it), _hash(h), index(i) {}
      iterator(const iterator& i):_it(i._it), _hash(i._hash), index(i.index) {};
      const Data& operator * () const { return *_it; }
      iterator& operator ++ () { 
         ++_it;
         while(_it == _hash->_buckets[index].end()) {
            if(index == _hash->_numBuckets-1)   return *this;
            ++index;
            _it = _hash->_buckets[index].begin();
         }
         return *this;
      }
      iterator& operator -- () {
         --_it;
         if(_it == _hash->_buckets[index].begin()) {
            if(index == 0) return *this;
            --index;
            while(_hash->_buckets[index].size() == 0) {
               if(index == 0) return *this;
               --index;
            }
            _it = _hash->_buckets[index].end();
            --_it;
         }
         return *this;
      }
      iterator operator ++ (int i) {
         iterator tmp = *this;
         ++(*this);
         return tmp;
      }
      iterator operator -- (int i) {
         iterator tmp = *this;
         --(*this);
         return tmp;
      }
      bool operator != (const iterator& i) const { 
         if(_it != i._it) return true;
         else return false; 
      }
      bool operator == (const iterator& i) const {
         if(_it == i._it) return true;
         else return false;
      }
      iterator& operator = (const iterator& i) const {
         _it = i._it;
         _hash = i._hash;
         index = i.index;
         return *this;
      }
   private:
      class vector<Data>::iterator _it;
      const HashSet* _hash;
      size_t index;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      for(size_t i = 0; i < _numBuckets; ++i) {
         if(!_buckets[i].empty())   return iterator(_buckets[i].begin(), this, i);
      }
      return end();
   }
   // Pass the end
   iterator end() const { return iterator(_buckets[_numBuckets-1].end(), this, _numBuckets-1); }
   // return true if no valid data
   bool empty() const { 
      for(size_t i = 0; i < _numBuckets; ++i) {
         if(!_buckets[i].empty()) return false;
      }
      return true; 
   }

   void print() const {
      for(size_t i = 0; i < _numBuckets; ++i) {
         cout << "bucket " << i << ": ";
         
         for(class vector<Data>::const_iterator it = _buckets[i].begin(); it != _buckets[i].end(); ++it){
            cout << *it << " ";
         }
         cout << endl;
      }
   }
   // number of valid data
   size_t size() const {
      size_t s = 0; 
      for(size_t i = 0; i < _numBuckets; ++i) {
         s += _buckets[i].size();
      }
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t id = bucketNum(d);
      for(size_t i = 0; i < _buckets[id].size(); ++i) {
         if(_buckets[id][i] == d) return true;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      size_t id = bucketNum(d);
      for(size_t i = 0; i < _buckets[id].size(); ++i) {
         if(_buckets[id][i] == d) { d = _buckets[id][i]; return true; }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t id = bucketNum(d);
      for(size_t i = 0; i < _buckets[id].size(); ++i) {
         if(_buckets[id][i] == d) { _buckets[id][i] = d; return true; }
      }
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      size_t id = bucketNum(d);
      for(size_t i = 0; i < _buckets[id].size(); ++i) {
         if(_buckets[id][i] == d) return false;
      }
      _buckets[id].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t id = bucketNum(d);
      for(size_t i = 0; i < _buckets[id].size(); ++i) {
         if(_buckets[id][i].getName() == d.getName()) { 
            class vector<Data>::iterator tmp = find(_buckets[id].begin(), _buckets[id].end(), d);
            _buckets[id].erase(tmp); 
            return true; 
         }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
