/**
 * 该头文件通过函数模板实现了大多数常用的基本算法。
 */

#ifndef __STL_ALGOBASE_H
#define __STL_ALGOBASE_H

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <new.h>

#include "../iterator/stl_iterator.h"
#include "../iterator/type_traits.h"

namespace my_tiny_stl{
/**
 * 我们强调，SGI STL是一个无所不用其及来追求效率的艺术品，一个很典型的例子就是基础算法copy的实现。
 * copy的底层实现就是通过赋值来完成的，但是有些对象是拥有trivial assignment operator的，也就是
 * 说这些对象的赋值操作本质上而言就是内存的拷贝，如果我们能够使用系统调用memmove、memcpy来完成拷贝
 * 任务的话，效率就会高很多。
 */


/**
 * 我们这里对c++中的引用再进行一个讨论。本质上将引用就是指针，是对指针的封装，使用起来更加安全。在c++
 * 中我们尽量使用引用，这时一种好的编程行为，除非你能对你的指针理解足够自信。我们的问题是传递参数的时候
 * 什么时候传递引用呢？传递引用有两个目的就跟c中的传递指针一样，第一个目的是效率较高，第二个目的是函数内部
 * 能够对相应的对象内容进行修改。通常，如果我们只是追求效率而不对对象内容进行修改的话，那么函数参数类型
 * 就可以是const type&。如果要对目标进行修改，可以是type&。对于第一种情况，任何情况下传递引用效率都是
 * 最好的吗？非也。什么情况下传递引用的效率竟然会低呢？当函数内部需要对象的拷贝的时候。如果传递引用，消耗有
 * 两步，第一步是指针的传递，第二步是对象的生成。而如果在传递参数的时候直接传递对象，则只有一步消耗。这就是
 * 为什么我们的copy实现传递不是引用。
 */

/**
 * 算法中另一个让我们迷惑的地放在于，拿copy举例，既然这个算法可以接受任意类型的迭代器，将参数名称命名为
 * Interator显得不是那么合理。单纯从名称的角度考虑，可以将参数命名为任何名称，这里命名为Interator的
 * 目的在于，告诉用户这个算法能接受的最低阶迭代器类型是InputIterator类型。这时STL的一个命名规则，这
 * 实际上是一个好的编程习惯，在模板编程中，参数的名称即可告诉用户，这个函数模板/类模板期待接受什么样的类型。
 */

/**
 * 再讨论一个问题，类模板包括泛化、偏特化、特化。函数模板包括泛化、特化。那么给定一个类模板生成的类以及一个
 * 通过函数模板生成的函数，我们怎么知道是由哪个模式的类模板或者函数模板生成的呢？很简单，对于类，我们从类的
 * 特化列表出发匹配即可。对于函数，从函数参数类型出发匹配即可。
 *
 * 上面所属似乎有点多余，模板匹配不就是这样吗，为什么还要单独拿出来强调？只有我们时时刻刻清楚这一点，才能写出
 * 正确的类模板或者函数模板。
 */

/**
 * **********************************copy start**************************************
 */
template<class T>
T* ___copy(T* first,
		   T* last,
		   T* result,
		   true_type){
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template<class T>
T* ___copy(const T* first,
		   const T* last,
		   T* result,
		   true_type){
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template<class T>
T* ___copy(T* first,
		   T* last,
		   T* result,
		   false_type){
	
	return ___copy(first, last, result);
}

template<class T>
T* ___copy(const T* first,
		   const T* last,
		   T* result,
		   false_type){
	
	return ___copy(first, last, result);
}

template<class RandomAccessIterator,
		 class Outerator>
Outerator* ___copy(RandomAccessIterator* first,
				   RandomAccessIterator* last,
				   Outerator* result){
	for(int n = last - first; n > 0; --n, ++first, ++result){
		*result = *first;
	}

	return result;
}


template<class Interator, class Outerator>
Outerator __copy_inner(Interator first,
					   Interator last,
					   Outerator result,
					   input_iterator_tag){

	for(;first != last; ++first, ++result){
		*result = *first;
	}

	return result;
}

template<class RandomAccessIterator, class Outerator>
Outerator __copy_inner(RandomAccessIterator first,
					   RandomAccessIterator last,
					   Outerator result,
					   random_access_iterator_tag){
	return ___copy(first, last, result);
}


template<class Interator, class Outerator>
class __copy{
public:
	static Outerator operator()(Interator first,
						 Interator last,
						 Outerator result){
		return __copy_inner(first, last, result, 
							iterator_traits<first>::iterator_category());
	}
};

/**
 * 一定要注意在模板编程中对类型const type与类型type的区别。这两者是不同的类型。在我们的copy例子中
 * 需要对原生指针类型进行特化。指针可以分为T*和const T*，这两者的特化一样吗？
 *
 * 如果我们只有如下的一次特化，当我们调用__copy<const int*, int*>，因为特化列表中两个参数不同所
 * 以就会进入到泛化的__copy中，这显然不是我们希望的，所以我们希望针对__copy<const T*, T*>再进行
 * 一次偏特化。
 */

template<class T>
class __copy<T*, T*>{
public:
	T* operator()(T* first,
				  T* last,
				  T* result){
		return ___copy(first, last, result, type_traits<T>::has_trivial_assignment_operator());
	}
};

template<class T>
class __copy<const T*, T*>{
public:
	T* operator()(const T* first,
				  const T* last,
				  T* result){
		return ___copy(first, last, result, type_traits<T>::has_trivial_assignment_operator());
	}
};

template<class Interator, class Outerator>
Outerator _copy(Interator first,
				Interator last,
				Outerator result){
	return __copy<Interator, Outerator>()();
}


inline char* _copy(const char* first, const char* last, char* result){
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* _copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}


template<class Interator, class Outerator>
Outerator copy(Interator first,
			   Interator last,
			   Outerator result){
	return _copy(first, last, result);
}

/**
 * ********************************copy end************************************
 */


/**
 * *******************************copy_backward start****************************
 */
template<class RandomAccessIterator1,
		 class RandomAccessIterator2>
RandomAccessIterator2 
___copy_backward_called(RandomAccessIterator1 first,
						RandomAccessIterator1 last,
						RandomAccessIterator2 result){
	ptrdiff_t num = last - first;

	for(;num > 0; --num){
		*--result = *--last;
	}

	return result - num;
}

template <class T>
T* ___copy_backward(T* first, T* last, T* result, true_type){
	ptrdiff_t num = last - first;
	memmove(result - num, first, sizeof(T) * num);

	return result - num;
}

template <class T>
T* ___copy_backward(T* first, T* last, T* result, false_type){
	return ___copy_backward_called(first, last, result);
}


template <class T>
T* ___copy_backward(const T* first, const T* last, T* result, true_type){
	ptrdiff_t num = last - first;
	memmove(result - num, first, sizeof(T) * num);

	return result - num;
}

template <class T>
T* ___copy_backward(const T* first, const T* last, T* result, false_type){
	return ___copy_backward_called(first, last, result);
}

template<class BidirectionalIterator1,
		 class BidirectionalIterator2>
BidirectionalIterator2
___copy_backward(BidirectionalIterator1 first,
				 BidirectionalIterator1 last,
				 BidirectionalIterator2 result,
				 bidirectional_iterator_tag){
	for(;last != first;)
		*--result = *--last;

	return result;
}

template<class RandomAccessIterator1,
		 class RandomAccessIterator2>
RandomAccessIterator2
___copy_backward(RandomAccessIterator1 first,
				 RandomAccessIterator1 last,
				 RandomAccessIterator2 result,
				 random_access_iterator_tag){
	return ___copy_backward_called(first, last, result);
}

template<class BidirectionalIterator1,
		 class BidirectionalIterator2>
class __copy_backward{
public:
	BidirectionalIterator2 operator()(BidirectionalIterator1 first,
									  BidirectionalIterator1 last,
									  BidirectionalIterator2 result){
		return ___copy_backward(first, last, result,
								iterator_traits<first>::iterator_category());
	}
};

template<class T>
class __copy_backward<T*, T*>{
public:
	T* operator()(T* first, T* last, T* result){
		return ___copy_backward(first, last, result,
								type_traits<T>::has_trivial_default_constructor());
	}
};

template<class T>
class __copy_backward<const T*, T*>{
public:
	T* operator()(const T* first, const T* last, T* result){
		return ___copy_backward(first, last, result,
								type_traits<T>::has_trivial_default_constructor());
	}
};

template<class BidirectionalIterator1,
		 class BidirectionalIterator2>
BidirectionalIterator2 _copy_backward(BidirectionalIterator1 first,
									  BidirectionalIterator1 last,
									  BidirectionalIterator2 result){
	return __copy_backward<BidirectionalIterator1,
						   BidirectionalIterator2>()();
}

inline char* _copy_backward(const char* first,
					 const char* last,
					 char* result){
	ptrdiff_t num = last - first;
	memmove(result - num, first, last);
	return  result - num;
}

inline wchar_t* _copy_backward(const wchar_t* first,
						const wchar_t* last,
						char* result){
	ptrdiff_t num = last - first;
	memmove(result - num, first, sizeof(wchar_t) * num);

	return result - num;
}

template<class BidirectionalIterator1,
		class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
									 BidirectionalIterator1 last,
									 BidirectionalIterator2 result){
	return _copy_backward(first, last, result);
}

/**
 * *****************************copy_backward end*********************************
 */


template<typename ForwardIterator, typename T>
ForwardIterator fill_n(ForwardIterator first, size_t n, const T& x){
	for(;n > 0; ++first, --n)
		*first = x;

	return first;
}

template<typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& x){
	for(;first != last; ++first)
		*first = x;
}

/**
 * 这里一定要注意equal只是比较[first1, last1)与[first2, first2 + last1 - first1)之间的元素
 * 是否相同，并不关心first2 + last1 - first1 是不是比last2大即不关心第二序列的元素是不是比第一
 * 序列少。当第二序列的元素较少时，将产生不可预料的结果。所以，在使用这个函数前，程序员必须要保证正确性。
 */
template<class Interator1, class Interator2>
bool equal(Interator1 first1, Interator1 last1, Interator2 first2){
	for(;first1 != last1; ++first1, ++first2)
		if(*first1 != *first2)
			return false;
		return true;
}

template<class Interator1, class Interator2, class BinaryPredicate>
bool equal(Interator1 first1, Interator1 last1, Interator2 first2,
		   BinaryPredicate binary_pred){
	for(;first1 != last1; ++first1, ++first2)
		if(!pred(*first1, *first2))
			return false;
		return true;
}


}//namespace my_tiny_stl  
#endif //__STL_ALGOBASE_h