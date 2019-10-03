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
	typedef Value value_type;
	typedef Value& reference;
private:
	HashFunc hash;
	EqualKey equal;
	GetKey get_key;

	vector<node*, Alloc> bunckets;
	size_type num_elements;

private:
	void init_bunckets(size_type n){
		bunckets.reserve(n);
		bunckets.insert(bunckets.end(), n, (node*)0);
	}

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

	hashtable(size_type n, const HashFunc& h, const EqualKey& e, const GetKey& g = GetKey()) : 
	hash(h), equal(e), get_key(g), num_elements(0){init_bunckets();}

	hashtable(const hashtable& h) : 
	hash(h.hash), equal(h.equal), get_key(h.get_key), num_elements(0)
	{copy_from(h);}

	pair<iterator, bool> insert_unqiue(const Value& x){
		resize(num_elements + 1);
		return insert_unqiue_noresize(x);
	}

	iterator insert_equal(const Value& x){
		resize(num_elements + 1);
		return insert_equal_noresize(x);
	}

	/**
	 * 这个函数主要是为stl_hash_map提供的
	 */
	reference find_or_insert(const Value& obj);

	pair<iterator, iterator> equal_range(const Key& k);
	//该函数是将所有键值为k的节点删除。
	size_type erase(const Key& k);
	//删除节点pos
	void erase(iterator pos);
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
	self& operator=(const self& par){
		if(*this != par){
			clear();
			hash = par.hash;
			get_key = par.get_key;
			equal = par.equal;
			copy_from(par);
		}

		return *this;
	}
	friend bool operator==<>(const hashtable& obj1, const hashtable& obj2);
	~hashtable(){clear();}
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

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::reference
hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::find_or_insert(const Value& obj){
	
	resize(num_elements + 1);
	size_type buncket = bkt_num(obj);
	node* first = bunckets[buncket];

	for(; first && !equal(get_key(first->val), get_key(obj)); first = first->next)
		;

	if(first)
		return first->val;

	node* tmp = create_node(obj);
	tmp->next = bunckets[buncket];
	bunckets[buncket] = tmp;
	++num_elements;

	return tmp->val;
}
/**
 * 这个函数返回键值与k相等的节点范围迭代器[first, last)
 */
template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
pair<typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::iterator,
	 typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::iterator>
hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::equal_range(const Key& k){
	size_type buncket = bkt_num(k);
	node* first = bunckets[buncket];

	for(; first; first = first->next){
		//找到第一个位置
		if(equal(get_key(first->val), k)){
			//开始寻找最后一个位置
			for(node* last = first->next; last && equal(get_key(last->val), k); last = last->next)
				;
			//最后一个位置为last
			if(last)
				return pair<iterator(first, this), iterator(last, this)>;
			//first之后本列表中所有元素键值都为k，则寻找下一个iterator
			for(++buncket; buncket < bunckets.size() && !bunckets[buncket]; ++buncket)
				;
			//后面一个元素都没有
			if(buncket == bunckets.size())
				return pair<iterator(first, this), end()>;
			//有元素
			return pair<iterator(first, this), iterator(bunckets[buncket], this)>;
		}
	}
	//未找到
	return pair<end(), end()>;
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

template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
bool operator==(const hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>& obj1,
				const hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>& obj2){

	/**
	 * 注意这里特别容易出错的地方是，对于友元函数，因为其不算是类作用域，所以如果想要使用类内部
	 * 定义的类型、符号或者其他，不能直接使用。
	 */
	
	typedef typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::node node;
	typedef typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::size_type size_type;
		
	size_type size_1 = obj1.bunckets.size();
	size_Type size_2 = obj2.bunckets.size();
	if(size_1 != size_2)
		return false; 

	for(size_type i = 0; i < size_1; ++i){
		node* cur_1 = obj1.bunckets[i];
		node* cur_2 = obj2.bunckets[i];
		for(;cur_1 && cur_2 && cur_1->val == cur_2->val; cur_1 = cur_1->next, cur_2 = cur_2->next)
			;

		if(cur_1 || cur_2)
			return false;
	}

	return true;
}

/**
 * 这个函数删除所有键值为k的节点。注意这里的实现技巧，因为第一个节点的删除与其他节点的删除操作是不一样的，
 * 所以本函数实现首先从第二个节点开始判断，当遍历完列表之后，再判断第一个节点。
 */
template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::size_type
hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::erase(const Key& k){
	size_type buncket = bkt_key_num(k);
	node* first = bunckets[buncket];
	node* cur;
	size_type ret = 0;

	if(first){
		node* next = first->next;

		while(next){
			if(equal(get_key(next->val), k)){
				first->next = next->next;
				destroy_node(next);
				next = first->next;
				--num_elements;
				++ret;
			}
			else{
				first = next;
				next = first->next;
			}
		}

		first = bunckets[buncket];
		if(equal(get_key(first->val), k)){
			bunckets[buncket] = first->next;
			destroy_node(first);
			--num_elements;
			++ret;
		}
	}

	return ret;	
}
/**
 * 注意这个函数只是删除pos指向的节点，而不是删除所有与pos键值相等的节点
 */
template<class Value, class Key, class HashFunc,
		 class GetKey, class EqualKey, class Alloc = alloc>
// typename hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::size_type
void hashtable<Value, Key, HashFunc, GetKey, EqualKey, Alloc>::erase(iterator pos){
	node* cur = pos.node_ptr;

	if(cur){
		size_type buncket = bkt_num(cur->val);
		node* first = bunckets[buncket];

		if(first == cur){
			bunckets[buncket] = first->next;
			destroy_node(first);
			--num_elements;
			return;
		}

		node* next = first->next;
		while(next){
			if(next == cur){
				first->next = next->next;
				destroy_node(next);
				--num_elements;
				return;
			}
			first = next;
			next = first->next;
		}
	}
}

}//namespace my_tiny_stl

#endif//_STL_HASHTABLE_H