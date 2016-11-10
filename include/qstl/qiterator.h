
#ifndef _QSTL_ITERATOR_H_
#define _QSTL_ITERATOR_H_

struct input_iterator_tag{};
struct bidirectional_iterator_tag{};

template<class T, class Distance> struct input_iterator{};

template<class T, class Distance>
inline input_iterator_tag
iterator_category(const input_iterator<T, Distance>&){
	return input_iterator_tag();
}

#endif //!_QSTL_ITERATOR_H_