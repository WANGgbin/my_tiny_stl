#ifndef __STL_DEQUEUE_H
#define __STL_DEQUEUE_H

namespace my_tiny_stl{

inline size_t _buffer_size(size_t BufSize, size_t PerSize){
	return BufSize > 0 ? BufSize : (PerSize > 512 ? size_t(1) : (512 / PerSize));
}

template<class T, size_t BufSize>
struct dequeue_iterator{
	typedef random_access_iterator_tag 	iterator_category;
	typedef ptrdiff_t 					difference_type;
	typedef T 							value_type;
	typedef T* 							pointer;
	typedef T& 							reference;
	typedef dequeue_iterator 			self;
	typedef T** 						map_pointer;
	typedef size_t 						size_type;

	pointer 	cur;
	pointer 	first;//指向当前缓冲区的头
	pointer 	last;//指向当前缓冲区的尾，这里需要特别注意，包括备用空间
	map_pointer node;

	static size_t buffer_size(){return _buffer_size(BufSize, sizeof(T));}
	void set_node(map_pointer next_node){
		first = *next_node;
		last = first + (difference_type)buffer_size();
		node = next_node;
	}
	reference operator*(){return *cur;}
	pointer operator->(){return &(operator*());}

	/**
	 * 函数结果可正可负。
	 */
	difference_type operator-(const self& x){
		return difference_type((node - x.node - 1) * buffer_size() + 
							   (cur - first) + (x.last - x.cur));
	}

	self& operator++(){
		++cur;
		if(cur == last){
			set_node(node + 1);
			cur = first;
		}

		return *this;
	}

	self operator++(int){
		self tmp = *this;
		++*this;//注意操作符的结合性
		return tmp;
	}

	self& operator--(){
		if(cur == first){
			set_node(node - 1);
			cur = last - 1;
		}
		else
			--cur;

		return *this;
	}

	self operator--(int){
		self tmp = *this;
		--*this;
		return tmp;
	}

	self& operator+=(difference_type n){
		difference_type offset = n + (cur - first);

		if(offset >= 0 && offset < difference_type(buffer_size())){
			cur += n;
		}                                                                                                                                                                                                                                                                     
		else{
			difference_type node_offset = offset > 0 ?
			offset / difference_type(buffer_size()) :
			-difference_type((-offset - 1) / buffer_size()) - 1;//不太理解。
		}

		set_node(node + node_offset);
		cur = first + (offset - node_offset * (difference_type)buffer_size())
		return *this;
	}

	self operator+(difference_type n) const {
		self tmp = *this;
		return tmp += n;//或者可以这么写return tmp.operator+=(n);
	}

	self& operator-=(difference_type n){
		return operator+=(-n);//或者可以这么写(*this) += -n;
	}

	self operator-(difference_type n) const {
		self tmp = *this;
		return tmp -= n;//return tmp.operator-=(n);
	}

	reference operator[](difference_type n){
		return *(*this + n);
	}

	bool operator==(const self& x){
		return x.cur == cur;
	}

	bool operator!=(const self& x){
		return !operator==(x);
	}

	bool operator<(const self& x){
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
  
	bool operator>(const self& x){
		return x < (*this);
	}

	bool operator<=(const self& x){
		return !(*this > x);
	}

	bool operator>=(const self& x){
		return !(*this < x);
	}
};

template<class T, class Alloc = alloc, size_t BufSize = 0>
class dequeue{
public:
	typedef T 										value_type;
	typedef dequeue_iterator<value_type, BufSize> 	iterator;
	typedef value_type*								pointer;
	typedef pointer* 								map_pointer;
	typedef size_t 									size_type;
	typedef value_type& 							reference;
	typedef simple_alloc<value_type>				data_allocator;
	typedef simple_alloc<pointer>					map_allocator;
private:
	iterator start;
	iterator finish;
	map_pointer map;
	size_type map_size;

public:
	reference operator[](difference_type n){
		return start[n];
	}

	iterator begin(){return start;}
	iterator end(){return finish;}

	reference front(){return *start;}
	/**
	 * 为何不是*(finish - 1)?
	 */
	reference back(){
		iterator tmp = finish;
		--tmp;
		return *tmp;
	}

	size_type size(){return finish - start;}
	size_type max_size(){return size_type(-1);}
	bool empty(){return start == finish;}

private:
	static size_t buffer_size(){return iterator::buffer_size();}
	pointer data_allocate(){return data_allocator::allocate(buffer_size());}

	void data_deallocate(pointer ptr){return data_allocator::deallocate(ptr, buffer_size());}

	map_pointer map_allocate(size_t n){return map_allocator::allocate(n);}

	void map_deallocate(map_pointer ptr, size_t n){return map_allocator::deallocate(ptr, n);}

	void create_nodes(map_pointer first, map_pointer last);

	void destroy_nodes(map_pointer first, map_pointer last);

	void create_map_and_nodes(size_type n);

	void fill_initialize(size_type n, const T& x);

	template<class Interator>
	void copy_initialize(Interator first, Interator last);

	void reallocate_map(size_type nodes_to_add, bool add_at_front);

	void reserve_map_at_back(size_type nodes_to_add = 1){
		if((finish.node - map + 1 + nodes_to_add) > map_size)
			reallocate_map(nodes_to_add, false);
	}

	void reserve_map_at_front(size_type nodes_to_add = 1){
		if( (start.node - map) < nodes_to_add)
			reallocate_map(nodes_to_add, true);
	}

	void push_back_aux(const T& x);
	void push_front_aux(const T& x);

	void pop_back_aux();
	void pop_front_aux();

	iterator insert_aux(iterator pos, const T& x);

public:
	/**
	 * push操作特殊的地方在于可能会导致分配新的缓冲区以及map的调整或重新分配，但是pop的操作只可能
	 * 导致释放缓冲区并不会调整map，这是两者的不同。
	 */
	void push_back(const T& x);
	void push_front(const T& x);

	void pop_back();
	void pop_front();

	void clear();

	iterator insert(iterator pos, const T& x);
	template<class Interator>
	void insert(iterator pos, Interator first, Interator last);

	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);

	dequeue(){
		create_map_and_nodes(0);
	}
	
	dequeue(size_t n, const T& x){
		fill_initialize(n, x):
	}

	explicit dequeue(size_t n){
		fill_initialize(n, T());
	}

	dequeue(const self& x){
		create_map_and_nodes(x.size());
		uninitialized_copy(x.begin(), x.end(), start);
	}

	template<class Interator>
	dequeue(Interator first, Interator last){
		copy_initialize(first, last);
	}
/**
 * 两个dequeue相等的标准是只要元素相等即可，并不要求对象成员相等！
 */
	self& operator=(const self& x){
		if(*this != x){
			size_t len = size();
			size_t len_x = x.size();

			if(len < len_x){
				copy(x.begin(), x.begin() + len, start);
				insert(finish, x.begin() + len, x.end());
			}
			else{
				iterator new_finish = copy(x.begin(), x.end(), start);
				erase(new_finish, finish);
			}
		}

		return *this;
	}

	~dequeue(){
		destroy(start, finish);
		destroy_nodes(start.node, finish.node + 1);
		map_deallocate(map, map_size);
	}
};

/**
 * 这里我们可以考虑一个问题，什么时候需要捕获异常呢？我们知道在我们自己的分配器中，如果分配内存失败的话
 * 会自己抛出bad_alloc异常，那么对于这个异常什么时候需要捕获呢？是否捕获取决于是否需要异常处理，在我们
 * 的create_nodes例子中，因为可能已经成功分配了若干内存，这时某个内存分配失败的话，很显然我们需要将之前
 * 成功分配的内存释放，所以这里是需要捕获异常的。而如果我们仅仅是调用一次内存分配的话，是不需要捕获异常的。
 */
template<class T, class ALloc, size_t BufSize>
void dequeue<T, Alloc, BufSize>::create_nodes(map_pointer first, map_pointer last){
	try{
		map_pointer old_first = first;
		for(size_t n = last - first; n > 0; ++first, --n)
			*first = data_allocate();
	}
	catch(...){
		destroy_nodes(old_first, first);
		throw;
	}

}

template<class T, class ALloc, size_t BufSize>
void dequeue<T, Alloc, BufSize>::destroy_nodes(map_pointer first, map_pointer last){
	for(size_t n = last - first; n > 0; ++first, --n)
		data_deallocate(first);
}

/**
 * 这个函数用来分配map内存以及缓冲区，并初始化数据成员的值。注意当n为0的时候，start和finish指向同一个
 * 位置。注意程序中的异常处理。
 */
template<class T, class Alloc, size_t BufSize>
void dequeue<T, Alloc, BufSize>::create_map_and_nodes(size_type n){
	size_type num_nodes = n / buffer_size() + 1;
	map_size = 8 > (num_nodes + 2) ? 8 : (num_nodes + 2);

	map = map_allocate(map_size);
	map_pointer first = map_pointer + (map_size - num_nodes) / 2;
	map_pointer last = first + num_nodes;

	try{
		create_nodes(first, last);
	}
	catch(...){
		map_deallocate(map, map_size);
		throw;
	}

	start.set_node(first);
	finish.set_node(last - 1);
	start.cur = start.first;
	finish.cur = finish.first + n % buffer_size();

}

template<class T, class Alloc, size_t BufSize>
void dequeue<T, Alloc, BufSize>::fill_initialize(size_type n, const T& x){
	create_map_and_nodes(n);
	map_pointer cur = start.node;
	try{
		for(;cur < finish.node; ++cur)
			uninitialized_fill(*cur, *cur + buffer_size(), x);
		uninitialized_fill(finish.last, finish.cur, x);
	}
	catch(...){
		destroy_nodes(start.node , finish.node + 1);
		throw;
	}
}

template<class T, class Alloc, size_t BufSize>
template<class Interator>
void dequeue<T, Alloc, BufSize>::copy_initialize(Interator first, Interator last){
	size_t distance = distance(first, last);
	create_map_and_nodes(distance);
	map_pointer cur = start.node;
	Interator tmp = first;;
	try{	
		for(;cur < finish.node; ++cur){
			advance(tmp, buffer_size());
			uninitialized_copy(first , tmp, *cur);
			//注意这里的迭代器类型是Interator，所以不能+=!!!
			// first += tmp;
			// tmp += buffer_size();
			first = tmp;
		}
	}
	catch(...){
		destroy_nodes(start.node, finish.node);
		throw;
	}
}
/**
 * 该函数用来调整map，注意只是加入新的节点并不会为这些节点分配相应的缓冲区。
 */   
template<class T, class Alloc, size_t BufSize>
void dequeue<T, Alloc, BufSize>::reallocate_map(size_t nodes_to_add, bool add_at_front){
	size_t old_size = finish - start + 1;
	size_t new_size = old_size + nodes_to_add;
	map_pointer new_start;
	/**
	 * 如果当前的map的长度大于new_size的2倍，则不需要重新分配map，只需调整位置即可。
	 */
	if(map_size > 2 * new_size){
		new_start = map + (map_size - new_size) / 2 + (add_at_front ? nodes_to_add : 0);
		copy(start.node, finish.node + 1, new_start);			
	}
	/**
	 * 否则需要重新分配map，重新分配的map的大小是多少呢？是原来的map的长度加上原来map长度与加入节点
	 * 个数的较大值再加上2,加2的目的是方便扩充。
	 */
	else{
		size_t new_map_size = map_size + (map_size > nodes_to_add ? map_size : nodes_to_add) + 2;
		map_pointer new_map = map_allocate(new_map_size);
		new_start = new_map + (new_map_size - new_size) / 2 + (add_at_front ? nodes_to_add : 0);
		copy(start.node, finish.node + 1, new_start);
		map_deallocate(map, map_size);
		map = new_map;
		map_size = new_map_size;
	}
	start.node = new_start;
	finish.node = new_start + old_size - 1;
} 

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::push_back_aux(const T& x){
	reserve_map_at_back();
	*(finish.node + 1) = data_allocate();

	try{
		construct(finish.cur, x);
		finish.set_node(finish.node + 1);
		finish.cur = finish.start;
	}
	catch(...){
		data_deallocate(*(finish.node + 1));
		throw;
	}
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::push_back(const T& x){
	if( (finish.last - finish.cur) > 1){
		construct(finish.cur++, x);
		return;
	}
	push_back_aux(x);
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::push_front_aux(const T& x){
	reserve_map_at_back();
	*(start.node - 1) = data_allocate();

	try{
		construct(*(start.node - 1), x);
		start.set_node(start.node - 1);	
		start.cur = start.last - 1;
	}
	catch(...){
		data_deallocate(*(start.node - 1));
		throw;
	}
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::push_front(const T& x){
	if(start.first != start.cur){
		construct(--start.cur, x);
		return;
	}
	push_front_aux(x);
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::pop_back_aux(){
	data_deallocate(finish.first);
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	destroy(finish.cur);
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::pop_back(){
	if(finish.last != finish.cur){
		destroy(--finish.cur);
		return;
	}
	pop_back_aux();
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::pop_front_aux(){
	destroy(start.cur);
	data_deallocate(start.first);
	start.set_node(start.node + 1);
	start.cur = start.first;
}

template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::pop_front(){
	if(start.cur + 1 != start.last){
		destroy(start.cur++);
		return;
	}
	pop_front_aux();
}

/**
 * 这里我们需要明白一点clear之后dequeue的状态是什么样的。只有一个缓冲区，没有元素，即finish = start.
 */
template<class T, class Alloc, class BufSize>
void dequeue<T, Alloc, BufSize>::clear(){
	for(map_pointer tmp = start.node + 1;
		tmp < finish.node; ++tmp){
		destroy(*tmp, *tmp + buffer_size());
		data_deallocate(*tmp);
	}

	if(start.node != finish.node){
		destroy(start.cur, start.last);
		destroy(finish.first, finish.cur);
		data_deallocate(finish.first);
	}
	else
		destroy(start.cur, finish.cur);

	finish = start;
}

/**
 * 注意是前插操作，dequeue的插入操作是通过push_front和push_back操作完成的。函数返回插入元素
 * 所在的位置。
 */
template<class T, class Alloc, class BufSize>
iterator dequeue<T, Alloc, BufSize>::insert(iterator pos, const T& x){
	if(pos == start){
		push_front(x);
		return start;
	}

	if(pos == finish){
		iterator tmp = finish;
		push_back(x);
		return --tmp;
	}

	return insert_aux(pos, x);
}

/**
 * 在dequeue内部插入也是通过push_back、push_front来完成的。
 */
template<class T, class Alloc, class BufSize>
iterator dequeue<T, Alloc, BufSize>::insert_aux(iterator pos, const T& x){
	difference_type distance_from_start = pos - start;

	if(distance_from_start > size() / 2){
		iterator old_finish = finish;
		iterator back = old_finish;
		--back;
		push_back(*(finish - 1));
		copy_backward(pos, back, old_finish);
	}
	else{
		iterator old_start = start;
		iterator forward = old_start;
		++forward;
		push_front(*start);
		copy(forward, pos, old_start);
	}

	*pos = x;
	return pos;
}

template<class T, class Alloc, class BufSize>
template<class Interator>
void dequeue<T, Alloc, BufSize>::iterator(iterator pos, Interator first, Interator last){
	size_t add_elements = last - first;
	size_t elements_after_pos;
	size_t elements_before_pos
	size_t add_nodes;
	size_t mod_elements;
	iterator new_finish, new_start;

	if( (pos - start) > (finish - pos)){
		if(add_elements >= (finish.last - finish.cur)){
			add_nodes = (add_elements - (finish.last - finish.cur)) / buffer_size() + 1;
			mod_elements = (add_elements - (finish.last - finish.cur)) % buffer_size();
			reserve_map_at_back(add_nodes);
			try{
				for(map_pointer cur = finish.node + 1, size_t i = 0; i < add_nodes; ++cur, ++i)
					*cur = data_allocate();
			}
			catch(...){
				destroy_nodes(finish.node + 1, cur);
				throw;
			}			
		}
		try{
			elements_after_pos = finish - pos;
			if(add_elements < elements_after_pos){
				new_finish = uninitialized_copy(finish - add_elements, finish, finish);
				copy_backward(pos, finish - add_elements, finish);
				copy(first, last, pos);
			}
			else{
				new_finish = uninitialized_copy(last - (add_elements - elements_after_pos), last, finish);
				new_finish = uninitialized_copy(pos, finish, new_finish);
				copy(first, last, pos);
			}
		}
		catch(...){
			destroy_nodes(finish.node + 1, finish.node + 1 + add_nodes);
			throw;
		}
		finish = new_finish;

	}
	else{
		if(add_elements > (start.cur - start.first)){
			add_nodes = (add_elements - (start.cur - start.first)) / buffer_size() + 1;
			mod_elements = (add_elements - (start.cur - start.first)) % buffer_size();
			reserve_map_at_front(add_nodes);
			try{
				for(map_pointer cur = start.node - 1, size_t i = 0; i < add_nodes; --cur, ++i)
					*cur = data_allocate();
			}
			catch(...){
				destroy_nodes(cur + 1, start.node - 1);
				throw;
			}			
		}
		elements_before_pos = pos - start;
		new_start = start - add_elements;
		try{
			if(add_elements < elements_before_pos){
				uninitialized_copy(first, last, new_start);
				copy(start + add_elements, pos, start);
				copy_backward(first, last, pos);
			}
			else{
				iterator tmp = uninitialized_copy(start, pos, new_start);	
				uninitialized_copy(first, first + add_elements - elements_before_pos, tmp);
				copy(first + add_elements - elements_before_pos, last, start );
			}
		}
		catch(...){
			destory(start.node - add_nodes, start.node);
			throw;
		}
		start = new_start;
	}
}


template<class T, class Alloc, class BufSize>
iterator dequeue<T, Alloc, BufSize>::erase(iterator pos){
	size_t distance_from_start = pos - start;

	if(distance_from_start < size() / 2){
		copy_backward(start, pos, pos + 1);
		pop_front();
	}
	else{
		copy(pos + 1, finish, pos);
		pop_back();
	}

	return start + distance_from_start;
}

template<class T, class Alloc, class BufSize>
iterator dequeue<T, Alloc, BufSize>::erase(iterator first, iterator last){
	if(first == start && last == finish){
		clear();
		return finish;
	}

	size_t distance_from_start = first - start;
	size_t n = last - first;
	iterator new_start, new_finish;
	if(distance_from_start < (size() - n) / 2){
		new_start = copy_backward(start, first, last);
		destroy(start, start + n);
		for(map_pointer cur = start.node; cur < new_start.node; ++cur)
			data_deallocate(*cur);
		start = new_start;
	}
	else{
		new_finish = copy(last, finish, first);
		destroy(new_finish, finish);
		for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
			data_deallocate(*cur);
		finish = new_finish;
	}

	return start + distance_from_start;
}

template<class T, class Alloc, class BufSize>
bool operator==(const dequeue<T, Alloc, BufSize>& x,
				const dequeue<T, Alloc, BufSize>& y){
	return y.size() == x.size() && equal(x.begin(), x.end(), y.begin());
}


}//namespace my_tiny_stl

#endif//__STL_DEQUEUE_H