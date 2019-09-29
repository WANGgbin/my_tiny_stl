#ifndef __STL_QUEUE_H
#define __STL_QUEUE_H

namespace my_tiny_stl{

/**
 * 注意这里是前置声明forward declarations。因为函数operator==与operator<是queue
 * 的友元函数。
 */
template<class T, class Sequence = dequeue<T>>
class queue;

template<class T, class Sequence>
bool operator==(const queue<T, Sequence>& obj1,
				const queue<T, Sequence>& obj2);

template<class T, class Sequence>
bool operator<(const queue<T, Sequence>& obj1,
			   const queue<T, Sequence>& obj2);

template<class T, class Sequence = dequeue<T>>
class queue{

friend bool operator==<>(const queue& obj1, const queue& obj2);
friend bool operator<<>(const queue& obj1, const queue& obj2);

public:
	typedef typename Sequence::value_type 	value_type;
	typedef typename Sequence::reference 	reference;
	typedef typename Sequence::size_type 	size_type;
private:
	Sequence c;

public:
	queue() : c(){}

	explicit queue(const Sequence& x) : c(x){}

	bool empty(){return c.empty();}

	size_type size(){return c.size();}

	reference front(){return c.front();}

	reference back(){return c.back();}

	void push(const value_type & x){c.push_back(x)};

	void pop(){c.pop_front();}

	//默认queue的拷贝构造、赋值与析构通过成员c完成，所以没有显示提供拷贝构造、赋值与析构。
};

template<class T, class Sequence>
bool operator==(const queue<T, Sequence>& obj1,
				const queue<T, Sequence>& obj2){
	return obj1.c == obj2.c;
}

template<class T, class Sequence>
bool operator!=(const queue<T, Sequence>& obj1,
				const queue<T, Sequence>& obj2){
	return !(operator==(obj1, obj2));
}

template<class T, class Sequence>
bool operator<(const queue<T, Sequence>& obj1,
			   const queue<T, Sequence>& obj2){
	return obj1.c < obj2.c;
}

template<class T, class Sequence>
bool operator>=(const queue<T, Sequence>& obj1,
			   const queue<T, Sequence>& obj2){
	return !operator<(obj1, obj2);
}

template<class T, class Sequence>
bool operator>(const queue<T, Sequence>& obj1,
			   const queue<T, Sequence>& obj2){
	return operator<(obj2, obj1);
}

template<class T, class Sequence>
bool operator<=(const queue<T, Sequence>& obj1,
			   const queue<T, Sequence>& obj2){
	return !operator>(obj2, obj1);
}

}// namespace my_tiny_stl

#endif //__STL_queue_H