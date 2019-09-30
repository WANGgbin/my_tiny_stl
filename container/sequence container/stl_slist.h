#ifndef STL_SLIST_H
#define _STL_SLIST_H

namespace my_tiny_stl{

struct slist_base_node{
	slist_base_node * next;
};

template<class T>
struct slist_node : public slist_base_node{
	T data;
};

inline slist_base_node * slist_make_link(
						 slist_base_node * prev_node,
						 slist_base_node * new_node){
	new_node->next = prev_node->next;
	prev_node->next = new_node;

	return new_node;
}

inline slist_base_node* slist_previous(
						slist_base_node* head,
						slist_base_node* node){
	while(head && head->next != node)
		head = head->next;

	return head;
}

inline size_t slist_len(slist_base_node* head){
	size_t result;

	for(result = 0; head != 0; ++result, head = head->next)
		;

	return result;
}

struct slist_iterator_base{
	typedef size_t size_type;
	typedef forward_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;

	slist_base_node * node;

	slist_iterator_base(slist_base_node* par = 0){node = par;}

	bool operator==(const slist_iterator_base& par) const {
		return node == par.node;
	}

	bool operator!=(const slist_iterator_base& par) const {
		return !operator==(par);
	}
}

template<class T>
struct slist_iterator : public slist_iterator_base{
	typedef T 	value_type;
	typedef T& 	reference;
	typedef T* 	pointer;
	typedef slist_iterator<T> self;

	slist_iterator(slist_base_node* par = 0) : slist_iterator_base(par){}


	self& operator++(){
		node = node->next;
		return *this;
	}

	self operator++(int){
		self tmp = *this;
		++*this;
		return tmp;
	}

	reference operator*(){
		return ((slist_node*)node)->data;
	}

	pointer operator->(){
		return &(operator*());
	}
};

template<class T, class Alloc = alloc>
class slist{
public:
	typedef slist<T, Alloc>		self;
	typedef T 					value_type;
	typedef simple_alloc<slist_node, Alloc> slist_node_allocator;
	typedef slist_node*			link_type;

private:
	slist_base_node head;

public:
	slist(){head->next = 0;}
	slist(size_t n, const T& x){
		insert_after(&head, n, x);
	}

	template<class Interator>
	slist(Interator first, Interator last){
		insert_after(&head, first, last);
	}

	slist(const self& par){
		insert_after(&head, par.begin(), par.end());
	}

	self& operator=(const self& x){}

	~slist(){clear();}

private:
	link_type get_node(){return slist_node_allocator::allocate();}
	void put_node(link_type ptr){slist_node_allocator::deallocate(ptr);}
	link_type create_node(const T& x){
		link_type ret = get_node();
		try{
			construct(&ret->data, x);
			ret->next = 0;
		}
		catch(...){
			put_node(ret);
			throw;
		}

		return ret;
	}

	void destroy_node(link_type ptr){
		destroy(&ptr->data);
		put_node(ptr);
	}

public:
	iterator insert_after(iterator pos, const T& x){
		link_type new_node = create_node(x);
		return (link_type)slist_make_link(pos.node, new_node);
	}

	iterator insert_after(iterator pos, size_t n, const T& x){
		for(; n > 0; --n)
			pos.node = slist_make_link(pos.node, create_node(x));
		return pos;
	}

	template<class Interator>
	iterator insert_after(iterator pos, Interator first, Interator last){
		for(;first != last; ++first){
			pos.node = slist_make_link(pos.node, create_node(*first));
		}

		return pos;
	}

	iterator erase_after(iterator pos){
		link_type next = (link_type)pos.node->next;
		pos.node->next = next->next;
		destroy_node(next);
		return (value_type)(pos.node->next);
	}

	/**
	 * 函数删除before~last_node之间的节点，不包括before与last_node指向的节点
	 */
	iterator erase_after(iterator before, iterator last_node){
		link_type cur = (link_type)(before.node->next);

		if(cur != last_node.node){
			link_type tmp = cur;
			cur = (link_type)cur->next;
			destroy_node(tmp);
		}

		return last_node;
	}

	iterator begin(){return head.next;}
	iterator end(){return iterator();}
	size_type size(){return slist_len(head.next);}
	value_type front(){return ((link_type)(head.next))->data;}
	void push_front(const T& x){slist_make_link(&head, create_node(x));}
	void pop_front(){erase(&head);}
	iterator previous(iterator pos){return slist_previous(&head, pos.node);}
	void clear();
};

template<class T, class Alloc>
self& slist<T, Alloc>::operator=(const self& x){
	if(*this != x){
		link_type ptr1 = (link_type)head.next;
		link_type ptr2 = (link_type)(x.begin().node);
		link_type tmp;

		while(ptr1 && ptr2){
			tmp = ptr1;
			ptr1.data = ptr2.data;
			ptr1 = (link_type)(ptr1->next);
			ptr2 = (link_type)(ptr2->next);
		}

		if(ptr1)
			erase_after(tmp, iterator());
		else
			insert_after(tmp, ptr2, iterator());
	}

	return *this;
}

template<class T, class Alloc>
void slist<T, Alloc>::clear(){
	link_type cur = (link_type)(head.next);

	while(cur){
		link_type tmp = cur;
		cur = (link_type)(cur->next);
		destroy_node(tmp);
	}

	head.next = 0;
}

template<class T, class Alloc>
bool operator==(const slist<T, Alloc>& obj1, 
				const slist<T, Alloc>& obj2){
	iterator first1 = obj1.begin();
	iterator last1 = obj1.end();
	iterator first2 = obj2.begin();
	iterator last2 = obj2.end();

	for(first1 != last1 && first2 != last2 && *first1 == *first2){
		++first1;
		++first2;
	}

	return first1 == last1 && first2 == last2;
}


template<class T, class Alloc>
bool operator!=(const slist<T, Alloc>& obj1, 
				const slist<T, Alloc>& obj2){
	return !(obj1 == obj2);
}
 
}//my_tiny_stl

#endif//STL_SLIST_H