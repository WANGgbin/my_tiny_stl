#ifndef __STL_HASH_SET_H
#define __STL_HASH_SET_H

#include "stl_hashtable.h"

namespace my_tiny_stl{

/**
 * 这里讨论几个我们需要特别注意的地方
 *
 * 1、hash_set默认是以equal_to为比较函数的，该仿函数是直接通过==比较是否相同的，所以
 * 对于一些不能通过==比较相同的类型，我们必须显示提供比较参数。
 *
 * 2、SGI STL只提供了几种常见类型的哈希函数，对于特殊类型的哈希函数，我们必须显示提供。
 */
template<class Value, class HashFunc = hash<Value>, class EqualKey = equal_to<Value>, class Alloc = alloc>
class hash_set{
public:
	typedef hashtable<Value, Value, HashFunc, identity<Value>, EqualKey, Alloc>	ht;

private:
	ht m_hashtable;

};
}//my_tiny_stl
#endif//__STL_HASH_SET_H