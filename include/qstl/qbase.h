
#ifndef _QSTL_BASE_
#define _QSTL_BASE_

template<class T1, class T2>
void construct(T1 *p, const T2& x)
{
    new(p) T1(x); //Just run the copy constructor
    //new(p) T1() will run the constructor
}

template<class T>
void destroy(T *t)
{
    t->~T();
}

template<class iterator>
void destroy(iterator first, iterator end)
{
    while(first != end){
        destroy(&*first++);
    }
}

template<class iterator, class T>
iterator uninitialized_filled_n(iterator begin, int n, const T& x)
{
    while(n-- > 0)
        construct(&*begin++, x);
    return begin;
}

template<class iterator1, class iterator2>
iterator1 uninitialized_copy_n(iterator1 begin1, iterator2 begin2, iterator2 end2)
{
    while(begin2 != end2)
        construct(&*begin1++, *begin2++);
    return begin1;
}

template<class iterator1, class iterator2>
void copy_backward(iterator1 begin, iterator1 end, iterator2 dst_end)
{
    while(begin != end)
        *--dst_end = *--end;
}

template<class iterator1, class iterator2>
iterator1 copy_n(iterator1 begin1, iterator2 begin2, size_t n)
{
    while(n-- > 0)
        *begin1++ = *begin2++;
    return begin1;
}

#endif //!_QSTL_BASE_
