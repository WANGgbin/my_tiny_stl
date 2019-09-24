#ifndef __STL_UNINITIALIZED_H
#define __STL_UNINITIALIZED_H

namespace my_tiny_stl{

/**
 * uninitialized_copy函数是用来初始化一段空间的。如果类型是POD的则直接是内存的拷贝，我们使用copy来
 * 完成，如果不是POD的则调用对象的构造函数。
 */

template<class Interator, class ForwardIterator>
ForwardIterator _uninitialized_copy(Interator first,
									Interator last,
									ForwardIterator result,
									true_type){
	return copy(first, last, result);
}

template<class Interator, class ForwardIterator>
ForwardIterator _uninitialized_copy(Interator first,
									Interator last,
									ForwardIterator result,
									false_type){
	try{
		ForwardIterator cur = result;
		for(;first != last; ++first, ++cur)
			construct(&*cur, *first);
		return cur;
	}
	catch(...){
		destroy(result, cur);
		throw;
	}
}


template<class Interator, class ForwardIterator>
ForwardIterator uninitialized_copy(Interator first,
								   Interator last,
								   ForwardIterator result){
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD;

	return _uninitialized_copy(first, last, result, is_POD());
}

inline char* uninitialized_copy(const char* first,
						 const char* last,
						 char * result){
	memmove(result, first, last);
	return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first,
						 const wchar_t* last,
						 wchar_t * result){
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

template<class ForwardIterator, class T>
void _uninitialized_fill(ForwardIterator first, 
								   	ForwardIterator last,
								   	const T& x,
								   	true_type){
	fill(first, last, x);
}

template<class ForwardIterator, class T>
void _uninitialized_fill(ForwardIterator first, 
								   	ForwardIterator last,
								   	const T& x,
								   	false_type){
	try{
		ForwardIterator cur = first;
		for(;last != cur; ++cur)
			construct(&*cur, x); 
	}
	catch(...){
		destroy(first, cur);
		throw;
	}
}


template<class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, 
								   ForwardIterator last,
								   const T& x){
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD;

	_uninitialized_fill(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
ForwardIterator _uninitialized_fill_n(ForwardIterator first, 
								   	  size_t n,
					   	   			  const T& x,
					   	  			  true_type){
	return fill_n(first, n, x);
}

/**
 * 注意在我们连续构造的时候，如果途中出现异常，我们需要将构造的对象全部释放。所以要有异常处理机制。
 */
template<class ForwardIterator, class T>
ForwardIterator _uninitialized_fill_n(ForwardIterator first, 
									  size_t n,
						   			  const T& x,
						   			  false_type){
	try{
		ForwardIterator cur = first;
		for(;n > 0; ++cur, --n)
			construct(&*cur, x); 
		return cur;
	}
	catch(...){
		destroy(first, cur);
		throw;
	}
}


template<class ForwardIterator, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, 
								     size_t n,
								     const T& x){
	typedef typename iterator_traits<ForwardIterator>::value_type value_type;
	typedef typename type_traits<value_type>::is_POD_type is_POD;

	return _uninitialized_fill_n(first, n, x, is_POD());
}


}

#endif //__STL_UNINITIALIZED_H