#ifndef __STL_PRIORITY_QUEUE_H
#define __STL_PRIORITY_QUEUE_H

#include "stl_heap.h"

namespace my_tiny_stl{

template<class T, class Sequence = vector<T>,
		 class Comp = less<typename Sequence::value_type> >
class priority_queue{
public:
	typedef typename Sequence::value_type  	value_type;
	typedef typename Sequence::size_type 	size_type;
	typedef typename Sequence::reference	reference;
private:
	Sequence c;
	Comp comp;

public:
	priority_queue(){}
	priority_queue(const Sequence& par_c, const Comp& par_comp) : 
				  c(par_c), comp(par_comp){make_heap(c.begin(), c.end(), comp);}
	explicit priority_queue(const Comp& par_comp) : comp(par_comp){}

	template<class Interator>
	priority_queue(Interator first, Interator last) : c(first, last){
		make_heap(c.begin(), c.end(), comp);
	}

	//拷贝构造、operator=以及析构操作，编译器提供的默认操作即可满足要求，所以没必要提供。

	bool empty() const {return c.empty();}
	size_type size() const {return c.size();}
	void push(const value_type & x){
		//注意在使用push_heap前，必须将元素先插入容器末端
		c.push_back(x);
		push_heap(c.begin(), c.end());
	}

	void pop(){
		pop_heap(c.begin(), c.end());
		c.pop_back();
	}

	const reference top() const {return c.front();}
};

}//namespace my_tiny_stl

#endif//__STL_PRIORITY_QUEUE_H