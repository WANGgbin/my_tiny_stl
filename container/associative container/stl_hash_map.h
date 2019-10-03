#ifndef _STL_HASH_MAP_H
#define _STL_HASH_MAP_H

namespace my_tiny_stl{

template<class T, class Key, class HashFunc = hash<Key>,
		 class EqualKey = equal_to<Key>, class Alloc = alloc>
class hash_map{
public:
	typedef <pair<Key, T>, Key, HashFunc,
			 select1st<pair<Key, T>>, EqualKey, Alloc>	ht;
	typedef pair<Key, T>	value_type;
private:
	hs m_hashtable;

public:
	/**
	 * 这个函数是map的特殊所在。
	 */
	T& operator[](const Key& k){
		return m_hashtable.find_or_insert(value_type(k, T())).second;
	}
};
}//my_tiny_stl
#endif//_STL_HASH_MAP_H