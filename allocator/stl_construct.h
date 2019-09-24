#ifndef __STL_CONSTRUCT_H
#define __STL_CONSTRUCT_H

#include <new.h>

namespace my_tiny_stl{

template<typename T1, typename T2>
void construct(T1* pointer, const T2& obj){
	new (pointer) T1(obj);
}

template<typename T>
void destroy(T* ptr){
	ptr->~T();
}

template<typename ForwardIterator>
void _destroy(ForwardIterator first,
			  ForwardIterator last,
			  true_type){
	//do nothing.
}

template<typename ForwardIterator>
void _destroy(ForwardIterator first,
			  ForwardIterator last,
			  false_type){
	for(;first != last; ++first)
		destroy(&*first);
}

template<typename ForwardIterator>
void destroy(ForwardIterator first,
			 ForwardIterator last){
	typedef typename iterator_traits<ForwardIterator>::value_type		value_type;
	typedef typename type_traits<value_type>::has_trivial_destructor	t;

	_destroy(first, last, t());
}

inline void destroy(char*, char*){}
inline void destroy(int*, int*){}
inline void destroy(long*, long*){}
inline void destroy(double*, double*){}

}
#endif//__STL_CONSTRUCT_H