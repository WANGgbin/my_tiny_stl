#ifndef _STL_HASHTABLE_H
#define _STL_HASHTABLE_H

namespace my_tiny_stl{

template<class Value>
struct hashtable_node{
	typedef hashtable_node<Value> self;
	self* next;
	Value val;
};

/**
 * 因为迭代器中使用到了hashtable，所以这里需要前向声明。
 */
template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
class hashtable;

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
struct hashtable_iterator{
	typedef hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>	hashtable;
	typedef hashtable_node<Value>	node;
	typedef Value 	value_type;
	typedef Value* 	pointer;
	typedef Value& 	reference;

	//注意hashtable 迭代器的类型是forward_iterator_tag.
	typedef forward_iterator_tag iterator_category;
	typedef ptrdiff_t 	difference_type;
	typedef hashtable_iterator<Value, Key, HashFunc, GetKey, EqualKey, Alloc> self;

	node* node_ptr;
	hashtable* hash_ptr;

	hashtable_iterator(node* par1 = 0, hashtable* par2 = 0){ node_ptr = par1; hast_ptr = par2;}
	reference operator*(){
		return node_ptr->val;
	}

	pointer operator->(){
		return &(operator*());
	}

	self& operator++(){

	}

	self operator++(int);

	bool operator==(const self& par){
		return node_ptr == par.node_ptr && hash_ptr == par.hash_ptr;
	}

	bool operator!=(const self& par){
		return node_ptr != par.node_ptr || hash_ptr != par.hash_ptr;
	}
};

/**
 * 枚举类型是不能取地址的，并没有分配内存。
 */
enum {__stl_num_primes = 28};

static const unsigned long __stl_prime_list[__stl_num_primes] =

{

  53ul,         97ul,         193ul,       389ul,       769ul,

  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,

  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,

  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,

  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 

  1610612741ul, 3221225473ul, 4294967291ul

};

inline unsigned long __stl_next_prime(unsigned long n){
	/**
	 * 注意这里的指针只能是const类型
	 */
	
	const unsigned long * first = __stl_prime_list;
	const unsigned long * last = __stl_prime_list + __stl_num_primes;
	const unsigned long * pos = lower_bound(first, last, n);

	return pos == last ? *(last - 1) : *pos;
}

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
bool operator==(const hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>& obj1,
				const hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>& obj1);

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
class hashtable{
public:
	typedef hashtable_iterator<Value, Key, HashFunc, GetKey, EqualKey, Alloc> iterator;
	typedef hashtable_node<Value> node;
	typedef size_t 	size_type;
	typedef simple_alloc<node>	node_allocator;
private:
	HashFunc hash;
	EqualKey equal;
	GetKey get_key;

	vector<node*, Alloc> bunckets;
	size_type num_elements;

private:
	node* get_node(){
		node* ret = node_allocator::allocate();
		return ret;
	}

	node* cretae_node(const Value& x){
		node* ret = get_node();
		construct(&(ret->val), x);
		return ret;
	}

	void put_node(node* ptr){
		node_allocator::deallocate(ptr);
	}

	void destroy_node(node* ptr){
		destroy(&(ptr->val));
		put_node(ptr);
	}
	//调整哈希表的大小，注意调整规则。
	void resize(unsigned long n);
	pair<iterator, bool> insert_unqiue_noresize(const Value& x);
	iterator insert_equal_noresize(const Value& x);
	size_type bkt_key_num(Key k, size_type n){
		return hash(k) % n;
	}

	size_type bkt_num(const Value& x){
		return bkt_key_num(get_key(x), bunckets.size());
	}

	size_type bkt_num(const Value& x, size_type n){
		return bkt_key_num(get_key(x), n);
	}

	void copy_from(const hashtable& h);
public:
	pair<iterator, bool> insert_unqiue(const Value& x){
		resize(num_elements + 1);
		return insert_unqiue_noresize(x);
	}

	iterator insert_equal(const Value& x){
		resize(num_elements + 1);
		return insert_equal_noresize(x);
	}

	void clear();

	iterator find(const Value& x){
		size_type buncket = bkt_num(x);
		node* first = bunckets[buncket];

		while(first && !equal(get_key(x), get_key(first->val)))
			first = first->next;

		return iterator(first, this);
	}

	size_type count(const Value& x){
		size_type buncket = bkt_num(x);
		node* first = bunckets[buncket];
		size_type reuslt = 0;
		
		while(first){
			if(equla(get_key(x), get_key(first->val)))
				++result;
			first = first->next;
		}

		return result;		
	}
};

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
void hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::resize(unsigned long n){
	unsigned long old_size = bunckets.size();
	unsigned long new_size = __stl_num_primes(n);
	//这一步向我们展示了哈希表大小的调整规则，当元素的个数大于哈希表的大小的时候，需要重新调整哈希表的大小。
	if(new_size > old_size){
		vector<node*> tmp(new_size, node*(0));
		for(unsigned long i = 0; i < old_size; ++i){
			node* first = bunckets[i];
			while(first){
				size_type new_buncket = bkt_num(first->val, new_size);
				bunckets[i] = first->next;
				first->next = tmp[];
				//注意下面这两者的顺序。
				tmp[] = first;
				first = bunckets[i];
			}
		}
		/**
		 * 这一步特别妙，展示了swap的一个经典的用法，乍一看可能会觉得这种写法很奇怪，实际在技术层面
		 * 没任何问题，swap将bunckets与tmp对象调换，两者所拥有的资源也发生调换。函数结束仍然析构
		 * 对象tmp，所以会将之前bunckets指向的资源释放。
		 */
		bunckets.swap(tmp);
	}
}

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
pair<typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::iterator, bool>
hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::insert_unqiue_noresize(const Value& x){
	size_type buncket = bkt_num(x);
	node * cur = bunckets[buncket];

	for(;cur; cur = cur->next)
		if(equal(get_key(x), get_key(cur->val)))
			return pair<iterator, bool>(iterator(cur, this), false);

	node* new_node = create_node(x);
	new_node->next = bunckets[buncket];
	bunckets[buncket] = new_node;

	++num_elements;

	return pair<iterator, bool>(iterator(bunckets[buncket], this), true);
}


template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::insert_equal_noresize(const Value& x){
	size_type buncket = bkt_num(x);
	node* cur = bunckets[buncket];
	node* new_node = create_node(x);

	for(;cur; cur = cur->next){
		if(equal(get_key(x), get_key(cur->val))){
			new_node->next = cur->next;
			cur->next = new_node;
			++num_elements;
			return iterator(new_node, this);
		}
	}

	new_node->next = bunckets[buncket];
	bunckets[buncket] = new_node;
	++num_elements;
	return iterator(new_node, this);
}

/**
 * 注意这里的clear函数只是释放了所有节点，并没有释放bunckets，这一点一定要注意。
 */
template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
void hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::clear(){
	size_type size = bunckets.size();
	size_type buncket;

	for(buncket = 0; buncket < size; ++buncket){
		node* first = bunckets[buncket];

		while(first){
			node* tmp = first;
			destroy_node(tmp);
			first = first->next;
		}

		bunckets[buncket] = (node*)0;
	}

	num_elements = 0;
}


template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
void hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::copy_from(const hashtable& h){
	size_type h_size = h.bunckets.size();
	bunckets.clear();
	bunckets.reserve(h_size);
	bunckets.insert(bunckets.end(), h_size, (node*)0);

	size_type h_buncket = 0;

	try{
		for(; h_buncket < h_size; ++h_buncket){
			node* first = h.bunckets[h_buncket];
			node* copy = bunckets[h_buncket];
			while(first){
				copy = new_node(first->val);
				copy = copy->next;
				first = first->next;
			}
			copy = (node*)0;
		}

		num_elements = h.num_elements;
	}
	catch(...){
		/**
		 * 如果出现异常，将以及创建好的所有节点释放。
		 */
		clear();
		throw;
	}
}


}//namespace my_tiny_stl

#endif//_STL_HASHTABLE_H