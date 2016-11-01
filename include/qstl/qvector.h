
#ifndef _QSTL_VECTOR_H_
#define _QSTL_VECTOR_H_

#include "qalloc.h"
#include "qbase.h"

template<class T /*, class Alloc = alloc*/>
class QVector{
private:
    typedef QAdvanced_alloc<true, 0> allocator;
    typedef T  value_type;
    typedef T* pointer;
public:
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef size_t size_type;

public:
    QVector():begin_(0), end_(0), reserve_(0){}
    QVector(size_type n){
        begin_ = (T*)allocator::allocate(sizeof(T)*n);
        uninitialized_filled_n(begin_, n, T());
        end_ = begin_ + n;
        reserve_ = end_;
    }
    QVector(size_type n, const T& value){
        begin_ = (T*)allocator::allocate(sizeof(T)*n);
        uninitialized_filled_n(begin_, n, value);// run 'construct()' inside, copy construcotr
        end_ = begin_ + n;//the type of begin and end is T*
        reserve_ = end_;
    }
    QVector(const QVector<T /*, Alloc*/>& x){
        begin_ = (T*)allocator::allocate(sizeof(T)*x.size());
        uninitialized_copy_n(begin_, x.begin(), x.end());
        end_ = begin_ + x.size();
        reserve_ = end_;
    }
    QVector(const_iterator begin, const_iterator end){
    }
    ~QVector(){
        while(begin_ != end_){
            destroy(&*begin_);
            allocator::deallocate(begin_++, sizeof(T));
        }
        while(begin_ != reserve_){
            allocator::deallocate(begin_++, sizeof(T));
        }
    }
    iterator begin() { return begin_; }
    const_iterator begin() const { return begin_; }
    iterator end() { return end_; }
    const_iterator end() const { return end_; }
    size_type size() const { return (size_type)(end_ - begin_); }
    size_type capacity() const { return (size_type)(reserve_ - begin_); }
    bool empty() const { return begin_ == end_; }
    reference operator[](size_type n){ return *(begin_ + n); }
    const_reference operator[](size_type n) const { return *(begin_ + n); }
    QVector<T /*, Alloc*/>& operator=(const QVector<T /*, Alloc*/>& x){
    }
    void reserve(size_type n){//not change the end, only reserve
        const size_type old_size = size();
        if(n > capacity()){//when n is <= reserve, we will do nothing
            iterator new_vc = (iterator)allocator::allocate(n);
            copy_n(new_vc, begin_, size());
            deallocate(begin_, size(), capacity());
            begin_ = new_vc;
            end_ = begin_ + old_size;
            reserve_ = begin_ + n;
        }
    }
    reference front() { return *begin(); }
    const_reference front() const { return *begin_; }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end_ - 1); }
    void push_back(const T& x){
        insert(end_, x);
    }
    //void swap(QVector<T /*, Alloc*/>& x){
    //}
    iterator insert(iterator position, const T& x){
        size_type n = position - begin();
        if(end_ != reserve_ && position == end_){
            construct(end_, x);//create a new object
            end_++;
        }else{
            insert_aux(position, x);//only used here!!!
        }
        return begin() + n;
    }
    iterator insert(iterator position){
        insert(position, T());
    }
    void insert(iterator position, const_iterator first, const_iterator last);
    void insert(iterator position, size_type n, const T& x);
    void pop_back(){
        end_--;
        destroy(end_);//memory not deallocated
    }
    void erase(iterator position){
        if(position != end_-1)
            copy_n(position, position+1, end_ - position);
        end_--;
        destroy(end_);//memory space is still available
    }
    void erase(iterator first, iterator last){
        const size_type old_size = size();
        const size_type new_size = old_size - (last - first);
        if(new_size == 0){
            destroy(first, last);
            end_ = begin_; //all the memory space are available
        }else{
            copy_n(first, last, end_ - last);
            size_t tmp_dis = last - first;
            end_ -= tmp_dis;
        }
    }
    void resize(size_type new_size, const T& x){
        if(new_size <= size()){
            erase(begin_ + new_size, end_);
        }else{
            insert(end_, new_size, x);//increased as insert
        }
    }
    void resize(size_type new_size){
        resize(new_size, T());
    }
    void clear(){ erase(begin_, end_); }
private:
    void insert_aux(iterator position, const T& x);
    void deallocate(iterator position, size_type p_e, size_type p_r);
    iterator begin_;
    iterator end_;
    iterator reserve_;
};

template<class T /*, Alloc*/>
void QVector<T /*, Alloc*/>::insert_aux(iterator position, const T& x)
{
   if(end_ != reserve_){//must be position != end, more space available
       copy_backward(position, end_, end_+1);
       T x_copy = x;//copy constructor
       *position = x_copy;//why not *position = x??? I don't know...
	   end_++;
   }else{//end == reserve or end == reserve && position == end
       const size_type old_size = size();
       const size_type new_size = old_size != 0 ? 2*old_size : 1;
       iterator new_vc = (iterator)allocator::allocate(new_size*sizeof(T));
       copy_n(new_vc, begin_, position - begin_);
       T x_copy = x;//new object construct
       size_type dis = (size_type)(position - begin_);
       *(new_vc + dis) = x_copy;
       if(position != end_)
           copy_n(new_vc + dis + 1, position, end_ - position);
       deallocate(begin_, size(), capacity());
       begin_ = new_vc;
       end_ = new_vc + old_size + 1;
       reserve_ = new_vc + new_size;
   }
}

template<class T /*,class Alloc = alloc*/>
void QVector<T /*, Alloc*/>::deallocate(iterator position, size_type p_e, size_type p_r)
{
    p_r -= p_e;
    while(p_e-- > 0){
        destroy(&*position);//the same as destroy(position) here...
        allocator::deallocate(position++, sizeof(T));
    }
    while(p_r-- > 0){
        allocator::deallocate(position++, sizeof(T));
    }
}

template<class T /*, class Alloc = alloc*/>
void QVector<T /*, Alloc*/>::insert(iterator position, size_type n, const T& x)
{
    if(n < (reserve_ - end_)){//still have enough space
        copy_backward(position, end_, end_ + n);
        uninitialized_filled_n(position, n, x);
        end_ += n;
    }else {//need more space now
        const size_type old_size = size();
        const size_type new_size = 2*(old_size + n);//Is this good?
        iterator new_vc = (iterator)allocator::allocate(new_size*sizeof(T));
        copy_n(new_vc, begin_, position - begin_);
        size_type tmp_dis = position - begin_;
        uninitialized_filled_n(new_vc + tmp_dis, n, x);
        copy_n(new_vc + tmp_dis + n, position, end_ - position);
        deallocate(begin_, size(), capacity());
        begin_ = new_vc;
        end_ = begin_ + old_size + n;
        reserve_ = begin_ + new_size;
    }
}

template<class T /*, class Alloc = alloc*/>
void QVector<T /*, Alloc*/>::insert(iterator position, const_iterator first, const_iterator last)
{
    size_type n = (size_type)(last - first);
    if(n < (end_ - begin_)){//reserve space is enough
        copy_backward(position, end_, end_ + n);
        uninitialized_copy_n(position, first, last);//construct new object
        end_ += n;
    }else{ //need more space
        const size_type old_size = size();
        const size_type new_size = 2*(old_size + n);
        iterator new_vc = (iterator)allocator::allocate(new_size*sizeof(T));
        copy_n(new_vc, begin_, position - begin_);
        uninitialized_copy_n(new_vc + position - begin_, first, last);
        copy_n(new_vc + position - begin_ + n, position, end_ - position);
        deallocate(begin_, size(), capacity());
        begin_ = new_vc;
        end_ = begin_ + old_size + n;
        reserve_ = begin_ + new_size;
    }
}

#endif //!_QSTL_VECTOR_H_
