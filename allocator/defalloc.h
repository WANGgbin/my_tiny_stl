#ifndef __DEFALLOC_H
#define __DEFALLOC_H

#include <new>
#include <cstdlib>
#include <algorithm>

namespace my_tiny_stl{
template<class T>
T* allocate(ptrdiff_t n, T*){
	T* ret;
	std::set_new_handler(0);
	/**
	 * 当调用函数operator new的时候，一定要记得强制类型转化。
	 */
	ret = (T*)::operator new(n * sizeof(T), std::nothrow);

	if(!ret){
		std::cerr << "" << std::endl;
		exit(1);
	}

	return ret;
}

template<class T>
void deallocate(T* ptr){
	::operator delete(ptr);
}

template<class T>
class allocator{
/**
 * 关于typedef定义的类型的访问控制，我们可能会顾虑一个问题，如果访问控制是private，那么内部定义函数
 * 的参数、返回值可以使用typedef定义的类型吗？答案是可以。
 */
public:
	typedef T 			value_type;
	typedef T* 			pointer;
	typedef const T* 	const_pointer;
	typedef T& 			reference;
	typedef const T& 	const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t 	difference_type;

public:
	static pointer allocate(size_type n){
		return ::allocate((difference_type)n, (pointer)0);
	}

	static void deallocate(pointer ptr){
		::deallocate(ptr);
	}
};

}//namespace my_tiny_stl

#endif	//__DEFALLOC_H