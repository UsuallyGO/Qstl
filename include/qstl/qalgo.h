
#ifndef _QSTL_ALGO_
#define _QSTL_ALGO_

template<class T>
inline const T& __median(const T& a, const T& b, const T& c)
{
    if(a < b)
        if(b < c)
            return b;
        else if(a < c)
            return c;
        else
            return a;
    else//b < a
        if(a < c)
            return a;
        else if(c < b)
            return b;
        else
            return c;
}

template<class T, class Compare>
inline const T& __median(const T& a, const T& b, const T& c, Compare comp)
{
    if(comp(a, b))//a>b
        if(comp(b, c))//a>b>c
            return b;
        else if(comp(c, a))//c>a>b
            return a;
        else
            return c;
    else//b>a
        if(comp(c, b))//c>b>a
            return b;
        else if(comp(a, c))//b>a>c
            return a;
        else
            return c;
}

template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
    while(first != last) f(*first++);
    return f;
}

template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while(first != last && *first != value) ++first;
    return first;
}

template<class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred)
{
    while(first != last && !pred(*first)) ++first;
    return first;
}

template<class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
{
    if(first == last) return last;
    ForwardIterator next = first;
    while(++next != last){
        if(*first == *next) return first;
        first = next;
    }
    return last;
}

template<class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
                              BinaryPredicate binary_pred)
{
    if(first == last) return last;
    ForwardIterator next = first;
    while(++next != last){
        if(binary_pred(*first, *next)) return first;
        first = next;
    }
    return last;
}

template<class InputIterator, class T, class Size>
void count(InputIterator first, InputIterator last, const T& value, Size& n)
{
    while(first != last)
        if(*first++ == value) ++n;
}

template<class InputIterator, class Predicate, class Size>
void count_if(InputIterator first, InputIterator last, Predicate pred, Size& n)
{
    while(first != last)
        if(pred(*first++)) ++n;
}

template<class ForwardIterator1, class ForwardIterator2, class Distance1,
         class Distance2>
ForwardIterator __search(ForwardIterator first1, ForwardIterator last1, 
                   ForwardIterator2 first2, ForwardIterator last2,
                   Distance1*, Distance2*)
{
    Distance1 d1 = 0;
    distance(first1, last1, d1);
    Distance2 d2 = 0;
    distance(first2, last2, d2);

    if(d1 < d2) return last1;//must can't find 'second' in 'first'
    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;

    while(current2 != last2)
        if(*current1++ != *current2++)
            if(d1-- == d2)//just skip the first + d1 elements in first
                return last1;
            else{
                current1 = ++first1;
                current2 = first2;
            }

    return (current2 == last2) ? first1 : last1;
}

template<class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator last1,
                               ForwardIterator2 first2, ForwardIterator last2)
{
    return __search(first1, last1, first2, last2, distance_type(first1),
                    distance_type(first2));
}

#endif //!_QSTL_ALGO_

