#ifndef __STL_HEAP_H
#define __STL_HEAP_H

namespace my_tiny_stl{

/**
 * push_heap算法的应用前提是：1、两个参数表示底部容器的首和尾2、新元素已经插入到容器尾部.
 * 类中的函数调用没有先后顺序，全局函数的声明要注意前后顺序。
 */

template<class RandomAccessIterator, class Distance, class T>
void _push_heap_aux(RandomAccessIterator first, Distance holeIndex,
					Distance topIndex, const T& x){
	Distance parent = (holeIndex - 1) / 2;

	while(parent >= topIndex && *(first + parent) < *(first + holeIndex)){
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}

	*(first + holeIndex) = x;
}

template<class RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last){
	_push_heap_aux(first, last - first - 1, 0, *(last - 1));
}

 

}//namespace my_tiny_stl

#endif//__STL_HEAP_H