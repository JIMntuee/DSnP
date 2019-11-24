/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node->_next;
         return *(this); 
      }
      iterator operator ++ (int) { 
         iterator tmp = *this;
         _node = _node->_next;
         return tmp; 
      }
      iterator& operator -- () { 
         _node = _node->_prev;
         return *(this); 
      }
      iterator operator -- (int) { 
         iterator tmp = *this;
         _node = _node->_prev;
         return tmp; 
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *(this); 
      }

      bool operator != (const iterator& i) const { 
         if(_node != i._node)  return true;
         else  return false; 
      }
      bool operator == (const iterator& i) const { 
         if(_node == i._node) return true;
         else  return false;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { if(begin() == end()) return true; else return false; }
   size_t size() const { 
      size_t size = 0;
      for(iterator tmp = begin(); tmp != end(); tmp++)   size++;
      return size;
   }

   void push_back(const T& x) { 
      DListNode<T>* End = end()._node;
      iterator prev = End->_prev;
      DListNode<T>* new_node = new DListNode<T>(x, prev._node, End);
      prev._node->_next = new_node;
      End->_prev = new_node;
      _head = End->_next;
      _isSorted = false;
   }
   void pop_front() { 
      DListNode<T>* tmp = _head->_next;
      end()._node->_next = tmp;
      tmp->_prev = end()._node;
      delete _head;
      _head = tmp;
   }
   void pop_back() { 
      DListNode<T>* prev = end()._node->_prev;
      DListNode<T>* pprev = prev->_prev;
      pprev->_next = end()._node;
      end()._node->_prev = pprev;
      delete prev;
      _head = end()._node->_next;
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(empty()) return false;
      if(pos._node == _head){
         _head = _head->_next;
      }
      pos._node->_prev->_next = pos._node->_next;
      pos._node->_next->_prev = pos._node->_prev;
      delete pos._node;
      return true;
   }

   bool erase(const T& x) { 
      iterator pos = find(x);
      if(pos == end())  return false;
      erase(pos);
      _head = end()._node->_next;
      return true;
   }

   iterator find(const T& x) {
      iterator pos = begin();
      for(; pos != end(); pos++){
         if(*pos == x)  break;
      }
      return pos; 
   }

   // delete all nodes except for the dummy node
   void clear() { 
      for(iterator tmp = begin(); tmp != end(); tmp++){
         delete tmp._node;
      }
      _head = end()._node;
      _head->_next = _head;
      _head->_prev = _head;
      _isSorted = false;
   }  

   void sort() const { 
      if(_isSorted)  return;
      size_t size = DList::size();
      for(size_t i = 0; i < size; i++){
         bubblesort(size-i);
      }
      _isSorted = true;
   }
   

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   void swap (DListNode<T>*& prev, DListNode<T>*& next) const {
      ////////////////// for swapping adjacent nodes
      assert(prev != _head);
      prev->_prev->_next = next;
      next->_next->_prev = prev;
      prev->_next = next->_next;
      next->_prev = prev->_prev;
      prev->_prev = next;
      next->_next = prev;
      // T tmp = prev->_data;
      // prev->_data = next->_data;
      // next->_data = tmp;
   }
   void bubblesort (size_t n) const {
      DListNode<T>* tmp1 = begin()._node;
      DListNode<T>* tmp2 = tmp1->_next;
      for(size_t i = 0; i < n-1; i++){
         if(tmp1->_data > tmp2->_data){
            if(tmp1 == _head) { //swapping _head, but _head is const
               T tmp = tmp1->_data;
               tmp1->_data = tmp2->_data;
               tmp2->_data = tmp;
               tmp1 = tmp1->_next;
               tmp2 = tmp2->_next;
            }
            else {
               swap(tmp1, tmp2);
               tmp2 = tmp1->_next;
            } 
         } 
         else{
            tmp1 = tmp1->_next;
            tmp2 = tmp2->_next;
         }
      }
   }
   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
