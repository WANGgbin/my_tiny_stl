#ifndef _STL_FUNCTION_H
#define _STL_FUNCTION_H

namespace my_tiny_stl{

/**
 * 关系类运算仿函数
 */

template<class T>
struct equal_to{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 == obj2;
	}
};

template<class T>
struct not_equal_to{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 != obj2;
	}
};

template<class T>
struct greater{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 > obj2;
	}
};

template<class T>
struct less{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 < obj2;
	}
};

template<class T>
struct greater_equal{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 >= obj2;
	}
};

template<class T>
struct less_equal{
	bool operator()(const T& obj1, const T& obj2){
		return obj1 <= obj2;
	}
};
/**
 * 这三个函数只是将参数原封不动的传回。
 */
template<class Value>
struct identity{
	const Value& operator()(const Value& x){
		return x;
	}
};

template<class Pair>
struct select1st{
	const typename Pair::first_type& operator()(const Pair& x){
		return x.first;
	}
};

template<class Pair>
struct select2ed{
	const typename Pair::second_type& operator()(const Pair& x){
		return x.second;
	}
};
}//my_tiny_stl
#endif//_STL_FUNCTION_H