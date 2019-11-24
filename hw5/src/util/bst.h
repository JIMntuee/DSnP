/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>
#include <utility>
#include <queue>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _left(l), _right(r) {}

   T              _data;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
public:
   // TODO: design your own class!!
   BSTree() { _root = 0; }
   ~BSTree() { clear(); delete _root; }
   class iterator { 
      friend class BSTree;

   public:
      iterator(const BSTree* t, BSTreeNode<T>* n=0): _node(n), _tree(t) {
         if(n == 0 || n == t->_root)  return;
         BSTreeNode<T>* tmp = t->_root;
         while(true){
            if(n->_data < tmp->_data){
               _trace.push( make_pair(tmp, 'l') );
               if(tmp->_left == 0 || tmp->_left == n)  return;
               else tmp = tmp->_left;
            }
            else if(n->_data >= tmp->_data){
               _trace.push( make_pair(tmp, 'r') );
               if(tmp->_right == 0 || tmp->_right ==n) return;
               else tmp = tmp->_right;
            }
         }
      }
      iterator(const iterator& i) : _node(i._node), _trace(i._trace), _tree(i._tree) { }
      ~iterator() { }

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ (){
         if(_node->_right != 0){ // has right children
            _trace.push( make_pair(_node, 'r'));
            _node = _node->_right;
            while(_node->_left != 0){
               _trace.push( make_pair(_node, 'l'));
               _node = _node->_left;
            }
            return *this;
         }
         else{
            iterator tmp = *this;
            while(!_trace.empty()) {
               if(_trace.top().second == 'r')   _trace.pop();
               else {
                  _node = _trace.top().first;
                  _trace.pop();
                  return *this;
               }
            }
            *this = tmp;
            _trace.push( make_pair(_node, 'r') );
            _node = 0;
            return *this;
         }
      }
      iterator operator ++(int) {
         iterator tmp = *this;
         if(_node->_right != 0) { // has right children
            _trace.push( make_pair(_node, 'r'));
            _node = _node->_right;
            while(_node->_left != 0){
               _trace.push( make_pair(_node, 'l'));
               _node = _node->_left;
            }
            return tmp;
         }
         else{
            while(!_trace.empty()) {
               if(_trace.top().second == 'r')   _trace.pop();
               else {
                  _node = _trace.top().first;
                  _trace.pop();
                  return tmp;
               }
            }
            *this = tmp;
            _trace.push( make_pair(_node, 'r') );
            _node = 0;
            return tmp;
         }
      }
      iterator& operator -- () {
         if( _tree->empty() ) return *this;
         else if(_node == 0){ // for end()
            _node = _trace.top().first;
            _trace.pop();
            return *this;
         }
         else if(_node->_left != 0) { // has left children
            _trace.push( make_pair(_node, 'l'));
            _node = _node->_left;
            while(_node->_right != 0) { 
               _trace.push( make_pair(_node, 'r') );
               _node = _node->_right;
            }
            return *this;
         } 
         else {
            iterator tmp = *this;
            while(!_trace.empty()){
               if(_trace.top().second == 'l')   _trace.pop();
               else {
                  _node = _trace.top().first;
                  _trace.pop();
                  return *this;
               }
            }
            *this = tmp;
            return *this;
         }
      }
      iterator operator -- (int) {
         iterator tmp = *this;
         if( _tree->empty() ) return tmp;
         else if(_node == 0){ // for end()
            _node = _trace.top().first;
            _trace.pop();
            return tmp;
         }
         else if(_node->_left != 0) { // has left children, find the biggest in the left subtree
            _trace.push( make_pair(_node, 'l'));
            _node = _node->_left;
            while(_node->_right != 0) { 
               _trace.push( make_pair(_node, 'r') );
               _node = _node->_right;
            }
            return tmp;
         } 
         else { 
            while(!_trace.empty()){
               if(_trace.top().second == 'l')   _trace.pop();
               else {
                  _node = _trace.top().first;
                  _trace.pop();
                  return tmp;
               }
            }
            *this = tmp;
            return tmp;
         }
      }
      iterator operator + (int i) const { 
         iterator tmp = *this;
         for(int j = 0; j < i; j++) { tmp++; }
         return tmp;
      }
      iterator& operator += (int i) { 
         for(int j = 0; j < i; j++) { (*this)++; }
         return *this;
      }
      iterator operator - (int i) const {
         iterator tmp = *this;
         for(int j = 0; j < i; j++) { tmp--; }
         return tmp;
      }
      iterator& operator -= (int i) {
         for(int j = 0; j < i; j++) { (*this)--; }
         return *this;
      }
      iterator& operator = (const iterator& i) { 
         _node = i._node;
         _trace = i._trace;
         _tree = i._tree;
         return *this;
      }
      bool operator != (const iterator& i) const {
         if(i._node != _node)   return true;
         else return false;
      }
      bool operator == (const iterator& i) const { 
         if(i._node == _node)  return true;
         else return false;
      }

   private:
      BSTreeNode<T>* _node;
      stack< pair<BSTreeNode<T>*, char> > _trace;
      const BSTree* _tree;
   };

   iterator begin() const {
      if(empty()) return iterator(this, 0);
      BSTreeNode<T>* tmp = _root;
      while(true) {
         if(tmp->_left != 0) tmp = tmp->_left;
         else break;
      }
      return iterator(this, tmp);
   }
   iterator end() const {
      if(empty()) return iterator(this, 0);
      BSTreeNode<T>* tmp = _root;
      while(true) {
         if(tmp->_right != 0) tmp = tmp->_right;
         else break;
      }
      iterator pos(this, tmp);
      pos._trace.push( make_pair(tmp, 'r') );
      pos._node = 0;
      return pos;
   }
   bool empty() const { 
      if(_root == 0) return true;
      else return false;
   }
   size_t size() const {
      size_t size = 0;
      for(iterator tmp = begin(); tmp != end(); tmp++)   size++; 
      return size;
   }

   void insert(const T& x) {
      BSTreeNode<T>* new_node = new BSTreeNode<T>(x);
      if(empty()){
         _root = new_node;
         return;
      } 
      BSTreeNode<T>* tmp = _root;
      while(true) {
         if(x < tmp->_data) {
            if(tmp->_left == 0) { tmp->_left = new_node;  return; }
            else tmp = tmp->_left;
         }
         else if(x > tmp->_data) {
            if(tmp->_right == 0) { tmp->_right = new_node;  return; }
            else tmp = tmp->_right;
         }
         else {
            new_node->_right = tmp->_right;
            tmp->_right = new_node;
            return;
         }
      }
   }
   void pop_front() { 
      if(empty()) return;
      iterator start = begin();
      if(start._node == _root)   _root = start._node->_right;
      else if(start._node->_right != 0) start._trace.top().first->_left = start._node->_right;
      else start._trace.top().first->_left = 0;
      delete start._node;
   }
   void pop_back() { 
      if(empty()) return;
      iterator last = end();
      last--;
      if(last._node == _root) _root = last._node->_left;
      else if(last._node->_left != 0) last._trace.top().first->_right = last._node->_left;
      else last._trace.top().first->_right = 0;
      delete last._node;
   }
   bool erase(iterator pos) {
      if(empty()) return false;

      if(pos._node->_right == 0 && pos._node->_left == 0) { // leaf node
         if(pos._trace.empty()) { // for only _root
            assert(pos._node == _root);
            delete pos._node;
            _root = 0;
            return true;
         } 
         else if(pos._trace.top().second == 'r')  pos._trace.top().first->_right = 0;
         else  pos._trace.top().first->_left = 0; // second == 'l'
         delete pos._node;
      }
      else if(pos._node->_right != 0 && pos._node->_left != 0) { // two children
         BSTreeNode<T>* successor = findsuccessor(pos._node);
         pos._node->_data = successor->_data;
         erase(iterator(this, successor));
      }
      else{ // one children
         if(pos._node->_right != 0) {
            if(pos._node == _root)  _root = pos._node->_right;
            else if(pos._trace.top().second == 'r')   pos._trace.top().first->_right = pos._node->_right;
            else  pos._trace.top().first->_left = pos._node->_right; // second == 'l'
         }
         else if(pos._node->_left != 0) {
            if(pos._node == _root)  _root = pos._node->_left;
            else if(pos._trace.top().second == 'r')   pos._trace.top().first->_right = pos._node->_left;
            else   pos._trace.top().first->_left = pos._node->_left; // second == 'l'
         }
         delete pos._node;
      }
      return true;
   }
   bool erase(const T& x) { 
      iterator pos = find(x);
      if(pos == end())  return false;
      else erase(pos);
      return true;
   }
   iterator find(const T& x) { 
      BSTreeNode<T>* tmp = _root;
      if(empty()) return end();
      while(true) {
         if(tmp->_data > x) {
            if(tmp->_left == 0)  return end();
            else  tmp = tmp->_left;
         }
         else if(tmp->_data < x) {
            if(tmp->_right == 0) return end();
            else  tmp = tmp->_right;
         }
         else  return iterator(this, tmp);
      }
      return iterator(this, tmp);
   }
   void clear() { 
      for(iterator pos = begin(); pos != end(); pos++){
         delete pos._node;
      }
      _root = 0;
   }
   void sort() { }
   void print() const { }


private:
   BSTreeNode<T>* _root;
   BSTreeNode<T>* findsuccessor( BSTreeNode<T>* n ){ // for erasing node in two children case
      n = n->_right;
      while(n->_left != 0) n = n->_left;
      return n;
   }
   
};

#endif // BST_H
