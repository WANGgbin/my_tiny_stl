#ifndef __STL_LIST_H
#define __STL_LIST_H

#include <stddef.h>

namespace my_tiny_stl{

struct list_node_base{
	struct list_node_base * prev;
	struct list_node_base * next;
};


template<class T>
struct list_node : public list_node_base{
	T data;
};

struct list_iterator_base{

	list_iterator_base(){ptr = 0;}

	list_iterator_base(list_node_base* par){
		ptr = par.ptr;
	}

	typedef bidirectional_iterator_tag  iterator_category;
	typedef ptrdiff_t 					difference_type;

	list_node_base * ptr;

	bool operator==(const list_iterator_base& par){
		return ptr == par.ptr;
	}

	bool operator!=(const list_iterator_base & par){
		return ptr != par.ptr;
	}

};

template<class T>
struct list_iterator : public list_iterator_base{
	typedef T 	value_type;
	typedef T*	pointer;
	typedef T& reference;
	typedef list_node<T> node;
	typedef list_iterator<T> self;

	list_iterator(){}
	list_iterator(node * par) : list_iterator_base(par){}
	list_iterator(const self par) : list_iterator_base(par){}

	self& operator++(){
		ptr = ptr->next;
		return *this;
	}

	self operator++(int){
		self tmp = *this;
		ptr = ptr->next;
		return tmp;
	}

	self& operator--(){
		ptr = ptr->prev;
		return *this;
	}

	self operator--(int){
		self tmp = *this;
		ptr = ptr->prev;
		return tmp;
	}

	reference operator*(){
		return ((list_node*)ptr)->data;
	}

	pointer operator->(){
		return &(operator*());
	}

};


template<class T, class Alloc = alloc>
class list{
public:
	typedef list_node<T>	list_node_t;
	typedef list_node_t* link_type;
	typedef list_iterator<T> iterator;
	typedef const list_iterator<T> const_iterator;
	typedef simple_alloc<list_node_t> data_allocator;
	typedef size_t size_type;

private:
	link_type node;

public:
	list(){empty_initialized();}

	list(size_t n, const T& x){
		empty_initialized();
		insert(end(), n, x);
	}

	list(size_t n){
		empty_initialized();
		insert(end(), n, T());
	}

	template<class Interator>
	list(Interator first, Interator last){
		empty_initialized();
		insert(end(), first, last);
	}

	/**
	 * 这里我们需要注意以下析构函数。析构函数中是通过调用clear函数来释放内存资源的。但是要注意的地方
	 * 在于clear并不会释放node指向的节点，所以在析构函数中除了调用clear，我们还需要释放node指向的
	 * 节点，这一点一定要注意，否则会造成内存泄漏。
	 */
	~list(){
		clear();
		put_node(node);
	}

	iterator begin(){return (link_type)(node->next);}
	iterator end(){return node;}
	bool empty(){return begin() == end();}
	size_type size(){
		return distance(begin(), end());
	}
	size_type max_size(){return size_type(-1);}

private:

	/**
	 * get_node, create_node, put_node, destroy_node.
	 */

	link_type get_node(){
		return data_allocator::allocate();
	}

	link_type create_node(const T& x){
		link_type tmp = get_node();
		construct(&tmp->data, x);
		tmp->prev = tmp->next = 0;

		return tmp;
	}

	void put_node(link_type ptr){
		data_allocator::deallocate(ptr);
	}

	void destroy_node(link_type ptr){
		destroy(&ptr->data);
		put_node(ptr);
	}

	void empty_initialized(){
		node = get_node();
		node->prev = node;
		node->next = node;
	}

	void transfer(iterator pos, iterator first, iterator last);

public:
	/**
	 * list最重要的三个操作分别是Insert、erase、traverse.
	 */
	
	/**
	 * [SGI STL中双链表的插入是前插操作]
	 * @param  pos [插入位置]
	 * @param  x   [插入节点的值]
	 * @return     [返回插入位置]
	 */
	iterator insert(iterator pos, const T& x){
		link_type tmp = create_node(x);
		tmp->next = pos.ptr;
		tmp->prev = pos.ptr->prev;
		tmp->prev->next = tmp;
		pos.ptr->prev = tmp;

		return tmp;//调用iterator(node*par);
	}

	iterator insert(iterator pos, size_t n, const T& x){
		for(;n > 0; --n)
			insert(pos, x);
	}

	iterator insert(iterator pos, Interator first, Interator last){
		for(;first != last;)
			insert(pos, *first++);
	}

	void push_front(const T& x){
		insert(begin(), x);
	}

	void push_back(const T& x){
		insert(end(), x);
	}

	iterator erase(iteartor pos){
		iterator ret((link_type)pos.ptr->next);
		pos.ptr->next->prev = pos.ptr->prev;
		pos.ptr->prev->next = pos.ptr->next;
		destroy_node((link_type)(pos.ptr));
		return ret;
	}

	/**
	 * 这两个pop函数特别需要注意的地方在于，如果当前链表是空的，则该操作将唯一的节点也删除，
	 * 这个时候list成员node指向的空间已经被释放，所以再引用该node的时候，就会发生段错误。
	 */
	void pop_back(){
		iterator tmp = end();
		erase(--tmp);
	}

	void pop_front(){
		erase(begin());
	}

	

	void remove(const T& x);
	void clear();
	self& operator=(const self& x);
	void unique();
	void resize(size_t new_size, const T& x);
	/**
	 * SGI STL提供了多个版本的splice(拼接)操作。
	 */
	
	/**
	 * [splice 将另外一个链表x插入到本链表的位置pos之前，注意前提条件是x非空。]
	 * @param pos [当前链表的当前位置]
	 * @param x   [待插入链表]
	 */
	void splice(iterator pos, self& x){
		if(!x.empty()){
			transfer(pos, x.begin(), x.end());
		}
	}

	/**
	 * [splice description]
	 * @param pos [待插入位置]
	 * @param x   [x与*this可以是同一个链表]
	 * @param i   [要插入的元素在链表x的位置]
	 */
	void splice(iterator pos, self&, iterator i){
		iterator j = i;
		++j;
		if(j == pos || i == pos) return;
		transfer(pos, i, j);
	}

	void splice(iterator pos, self&, iterator first, iterator last){
		if(first != last){
			transfer(pos, first, last);
		}
	}


	void merge(self& x);
	void reverse();
	void sort();

};

/**
 * clear函数我们也可以通过反复调用earse函数来实现，但这样效率会很低。原因之一在于调用函数的入栈、出栈操作 。
 * 原因二在于earse中的一些指针操作在clear中其实是没有必要的。
 */
template<class T, class Alloc>
void list<T, Alloc>::clear(){
	link_type cur = (link_type)(node->next);
	while(cur != node){
		link_type tmp = cur;
		cur = (link_type)(cur->next);
		destroy_node(tmp);
	}

	node->next = node;
	node->prev = node;
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T& x){
	iterator first = begin();
	iterator last = end();

	for(;first != last;){
		if(*first == x)
			erase(first++);
		else
			++first;
	}
	
}

/**
 * 注意列表的赋值操作后面的思想。我们想当然的想法是将this链表先clear，然后再将x的所有元素
 * 插入到this中，但是STL中的思想是将x的元素一一赋值给this的链表，如果x的长度长的话，将x剩
 * 余的插入到this中去，如果this长，将多余的部分erase。这样相比我们的做法效率会高，我们的
 * 做法中会有节点的释放、节点的分配，而在SGT STL的实现中，抛开最后的erase或者insert不讲，
 * 中间的循环处理部分只有节点对应数据成员的赋值。
 */
template<class T, class Alloc>
self& list<T, Alloc>::operator=(const self& x){
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.begin();

	while(first1 != last1 && first2 != last2){
		*first1++ = *first2++;
	}

	if(first2 == last2)
		erase(first1, last1);
	else
		insert(last1, first2, last2);

	return *this;
}

/**
 * 注意这个函数并不是说让双链表中所有的元素都唯一，只是让连续且相同的元素保持唯一性。所以要让链表
 * 中的元素保持唯一的话，我们可以先对链表进行一次排序，然后调用unique函数。
 */
template<class T, class Alloc>
void list<T, Alloc>::unique(){
	iterator first = begin();
	iterator last = end();
	iterator next = first;

	while(++next != last){
		if(*first == *next)
			erase(next);
		else
			first = next;

		next = first;
	}
}

/**
 * 这个函数的功能是用来调整链表长度的，当当前链表的长度小于参数指定的长度的时候，则需要插入新的元素，当
 * 当前链表的长度大于参数指定的长度的时候，则需要删除多余的节点。
 */

template<class T, class Alloc>
void list<T, Alloc>::resize(size_t new_size, const T& x){
	iterator first = begin();
	iterator last = end();
	size_t len = 0;

	for(;first != last && len < new_size; ++first, ++len)
		;//注意这时一个良好的编程习惯，我们之所以把分号写在这儿，就是为了突出显示这个for循环什么都不做。

	if(first == last)
		insert(last, new_size - len, x);
	else
		erase(first, last);

}
/**
 * 这个函数要注意一个地方，pos不能位于first到last之间。这个函数可以作用于同一条双链表上，也可以作用于不同的双链表上。
 * 这个函数涉及到6个指针的变化，只需要注意指针变化的顺序即可。
 */
template<class T, class Alloc>
void list<T, Alloc>::transfer(iterator pos, iterator first, iteraotr last){
	if(pos != last){
		list_node_base * prev = last.ptr->prev;
		first.ptr->prev = last.ptr;
		last.ptr->prev = first.ptr->prev;
		prev->next = pos.ptr;
		first.ptr->prev = pos.ptr->prev;
		pos.ptr->prev->next = first.ptr;
		pos.ptr->prev = prev;
	}
}

/**
 * 该函数的作用是将链表x归并到本链表上，注意一个大的前提：两个链表都是递增排序的。注意是递增。
 */
template<class T, class Alloc>
void list<T, Alloc>::merge(self& x){
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();
	iterator next2;

	while(first1 != last1 && first2 != last2){
		if(*first1 > *first2){
			next2 = first2;
			transfer(first1, first2, ++next2);
			++first2;
		}
		else
			++first1;
	}

	if(first2 != last2)
		transfer(last1, first2, last2);
}
	
/**
 * 该函数的作用是把链表翻转，注意思路：将第2个~第n个元素依次插入到第一个元素前面，完成翻转。
 */
template<class T, class Alloc>
void list<T, Alloc>::reverse(){
	iterator first;
	iterator last = end();
	iterator next;
	//当有一个元素或者没有元素的时候，直接返回。
	if(node->next == node || node->next->next == node) return;
	
	first = begin();
	++first;
	next = first;
	++next;
	while(first != last){
		transfer(begin(), first++, next++);
	}
}

/**
 * 这时双链表最复杂的操作。因为双链表的迭代器是bidirectional类型的，而algrithom中提供的sort是
 * 要求random access的，所以这里必须提供自己的sort。
 */

template<class T, class Alloc>
void list<T, Alloc>::sort(){

}

template<class T, class Alloc>
bool operator==(const list<T, Alloc>& obj1,
				const list<T, Alloc>& obj2){
	typedef typename list<T, Alloc>::const_iterator const_iterator;

	const_iterator first1  = obj1.begin();
	const_iterator last1 = obj1.end();
	const_iterator first2 = obj2.begin();
	const_iterator last2 = obj2.end();

	for(;first1 != last1 &&
		 first2 != last2 &&
		 *first1 == *first2){
		++first1;
		++first2;
	}

	return first1 == last1 && first2 == last2;

}

}
#endif //STL_LIST_H