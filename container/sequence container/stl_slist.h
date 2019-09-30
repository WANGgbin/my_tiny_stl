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

	slist_iterator(slist_node* par) : slist_iterator_base(par){}
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
}//my_tiny_stl

#endif//STL_SLIST_H