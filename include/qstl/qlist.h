
#ifndef _QSTL_LIST_
#define _QSTL_LIST_

#include "qalloc.h"
#include "qbase.h"

template<class T>
struct _list_node{
    _list_node *next;
    _list_node *prev;
    T data;
};

template<class T>
struct _list_iterator{
    typedef T& reference;
    typedef _list_node<T>* link_type;
    typedef _list_iterator<T> iterator;

    link_type node;

    _list_iterator(link_type x):node(x){}
    _list_iterator():node(NULL){}
    reference operator*() const{
        return node->data;
    }
    iterator operator++(){//++ before
        node = node->next;
        return *this;
    }
    iterator operator++(int){//++ after
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    iterator operator--(){//-- before
        node = node->prev;
        return *this;
    }
    iterator operator--(int){//-- after
        iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator!=(iterator iter) const {
        return this->node != iter.node;
    }

    bool operator==(iterator iter) const {
        return this->node == iter.node;
    }
};

template<class T>
struct _list_const_iterator{
    typedef _list_node<T>* link_type;
    typedef const T& const_reference;
    typedef _list_const_iterator<T> const_iterator;

    link_type node;
    _list_const_iterator(link_type x):node(x){}
    _list_const_iterator():node(NULL){}
    const_reference operator*() const{
        return node->data;
    }
    const_iterator operator++(){//++ before
        node = node->next;
        return *this;
    }
    const_iterator operator++(int){//++ after
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    const_iterator operator--(){//-- before
        node = node->prev;
        return *this;
    }
    const_iterator operator--(int){//-- after
        const_iterator tmp = *this;
        --*this;
        return tmp;
    }
    bool operator!=(const_iterator iter) const {
        return this->node != iter.node;
    }
    bool operator==(const_iterator iter) const {
        return this->node == iter.node;
    }
};

template<class T /*, class Alloc = alloc*/>
class QList{
    typedef T  value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef _list_node<T>  node_type;
    typedef _list_node<T>* link_type;
    typedef QAdvanced_alloc<false, 0> allocator;
public:
    typedef _list_iterator<T> iterator;
    typedef _list_const_iterator<T> const_iterator;
    link_type get_node(){
        return (link_type)allocator::allocate(sizeof(node_type));
    }
    void put_node(link_type p){
        allocator::deallocate(p, sizeof(node_type));
    }
    QList() : length(0){
        dummy = get_node();
        dummy->next = dummy;
        dummy->prev = dummy;
        dummy->data = T();
    }
    iterator insert(iterator position, const T& x){
        link_type new_node = get_node();
        construct(&new_node->data, x);
        new_node->next = position.node;
        new_node->prev = (position.node)->prev;
        ((position.node)->prev)->next = new_node;
        (position.node)->prev = new_node;
        length++;
        return --position;
    }
    iterator insert(iterator position, size_type n, const T& x){
        while(n-- > 0)
            insert(position, x);
    }
    reference front(){ return *begin(); }
    const_reference front() const { return *begin(); }
    reference back(){ return *(--end()); }
    const_reference back() const { return *(--end()); }
    iterator begin() { return iterator(dummy->next); }
    const_iterator begin() const { return const_iterator(dummy->next); }
    iterator end() { return iterator(dummy); }
    const_iterator end() const { return const_iterator(dummy); }
    bool empty() const { return 0 == length; }
    size_type size() const { return length; }
    void pop_back(){
        iterator tmp = end();
        erase(--tmp);
    }
    void pop_front(){
        erase(begin());
    }
    void push_back(const T& x){
        insert(end(), x);
    }
    void push_front(const T& x){
        insert(begin(), x);
    }
    void remove(const T& x){
        iterator first = begin();
        iterator last = end();
        while(first != last){
            iterator tmp = first++;
            if(*tmp == x){ erase(tmp); break; }
        }
    }
    void erase(iterator first, iterator last){
        while(first != last){
            erase(first++);
        }
    }
    void erase(iterator position){
        if(position.node != dummy){
            position.node->prev->next = position.node->next;
            position.node->next->prev = position.node->prev;
            put_node(position.node);
            length--;
        }
    }
private:
    size_type length;
    link_type dummy;//dummy->next is the real first node
};

#endif //!_QSTL_LIST_H_
