#ifndef __STL_STACK_H
#define __STL_STACK_H

namespace my_tiny_stl{

/**
 * 注意这里是前置声明forward declarations。因为函数operator==与operator<是stack
 * 的友元函数。
 */
template<class T, class Sequence = dequeue<T>>
class stack;

template<class T, class Sequence>
bool operator==(const stack<T, Sequence>& obj1,
				const stack<T, Sequence>& obj2);

template<class T, class Sequence>
bool operator<(const stack<T, Sequence>& obj1,
			   const stack<T, Sequence>& obj2);

template<class T, class Sequence = dequeue<T>>
class stack{

friend bool operator==<>(const stack& obj1, const stack& obj2);
friend bool operator<<>(const stack& obj1, const stack& obj2);

public:
	typedef typename Sequence::value_type 	value_type;
	typedef typename Sequence::reference 	reference;
	typedef typename Sequence::size_type 	size_type;
private:
	Sequence c;

public:
	stack() : c(){}

	explicit stack(const Sequence& x) : c(x){}

	bool empty(){return c.empty();}

	size_type size(){return c.size();}

	reference top(){return c.back();}

	void push(const value_type & x){c.push_back(x)};

	void pop(){c.pop_back();}

	//默认stack的拷贝构造、赋值与析构通过成员c完成，所以没有显示提供拷贝构造、赋值与析构。
};

template<class T, class Sequence>
bool operator==(const stack<T, Sequence>& obj1,
				const stack<T, Sequence>& obj2){
	return obj1.c == obj2.c;
}

template<class T, class Sequence>
bool operator!=(const stack<T, Sequence>& obj1,
				const stack<T, Sequence>& obj2){
	return !(operator==(obj1, obj2));
}

template<class T, class Sequence>
bool operator<(const stack<T, Sequence>& obj1,
			   const stack<T, Sequence>& obj2){
	return obj1.c < obj2.c;
}

template<class T, class Sequence>
bool operator>=(const stack<T, Sequence>& obj1,
			   const stack<T, Sequence>& obj2){
	return !operator<(obj1, obj2);
}

template<class T, class Sequence>
bool operator>(const stack<T, Sequence>& obj1,
			   const stack<T, Sequence>& obj2){
	return operator<(obj2, obj1);
}

template<class T, class Sequence>
bool operator<=(const stack<T, Sequence>& obj1,
			   const stack<T, Sequence>& obj2){
	return !operator>(obj2, obj1);
}

}// namespace my_tiny_stl

#endif //__STL_STACK_H