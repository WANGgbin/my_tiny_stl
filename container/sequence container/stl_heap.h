#ifndef __STL_HEAP_H
#define __STL_HEAP_H

namespace my_tiny_stl{

/**
 * 对于完全二叉树，如果节点索引从0开始，则对于某个节点索引为i，则其父节点索引为(i - 1) / 2。
 * 其左孩子节点索引为2 * i + 1，右孩子节点索引为2 * i + 2。如果节点索引是从1开始的，父节点
 * 索引为i / 2，左孩子索引为2 * i，右孩子索引为2 * i + 1。
 */

/**
 * push_heap算法的应用前提是：1、两个参数表示底部容器的首和尾2、新元素已经插入到容器尾部.
 * 类中的函数调用没有先后顺序，全局函数的声明要注意前后顺序。
 */

template<class RandomAccessIterator, class Distance, class T, class Comp>
void _push_heap_aux(RandomAccessIterator first, Distance holeIndex,
					Distance topIndex, const T& x, Comp cmp){
	Distance parent = (holeIndex - 1) / 2;

	/**
	 * 这个条件是本算法最重要的地方。 
	 */
	while(holeIndex > topIndex && cmp(*(first + parent), x)){
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}

	*(first + holeIndex) = x;
}

template<class RandomAccessIterator, class Comp>
void push_heap(RandomAccessIterator first, RandomAccessIterator last, Comp cmp){
	typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
	_push_heap_aux(first, difference_type(last - first - 1), difference_type(0), *(last - 1), cmp);
}

/**
 * 当一个树的左右子树都是堆的时候，我们可以通过调用_adjust_heap来使得整个树满足堆的特性。参数
 * holeIndex表示根节点的索引。
 */
template<class RandomAccessIterator, class Distance, class T, class Comp>
void _adjust_heap(RandomAccessIterator first,
				  Distance holeIndex,
				  Distance len, const T& x,
				  Comp cmp){
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * holeIndex + 2;

	while(secondChild < len){
		if(cmp(*(first + secondChild), *(first + secondChild - 1)))
			--secondChild;
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * secondChild + 2;
	}
	//没有右子节点只有左孩子节点。
	if(secondChild == len){
		*(first + holeIndex) = *(first + secondChild - 1);
		holeIndex = secondChild - 1;
	}

	//下溯完毕，开始上溯。
	_push_heap_aux(first, holeIndex, topIndex, x, cmp);
}

/**
 * 注意：pop_heap之后，最大元素只是被置于底部容器的尾端，尚未取走。
 */
template<class RandomAccessIterator, class Comp>
void pop_heap(RandomAccessIterator first,
			  RandomAccessIterator last,
			  Comp cmp){
	typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
	typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;

	value_type value = *(last - 1);
	*(last - 1) = *(first);
	__adjust_heap(first, difference_type(0), difference_type((last - first - 1)), value, cmp);
}

/**
 * sort_heap排序的思想是，既然每次pop_heap是将最大的元素至于容器尾端，那么通过反复调用pop_heap即可
 * 完成元素排序。
 */
template<class RandomAccessIterator, class Comp>
void sort_heap(RandomAccessIterator first,
	 		   RandomAccessIterator last,
	 		   Comp cmp){
	for(;last - 1 > first; --last)
		pop_heap(first, last, cmp);
}

/**
 * 制作堆的思路是从第一个非叶子节点开始，通过不断的调用adjust_heap来调整堆。
 */
template<class RandomAccessIterator, class Comp>
void make_heap(RandomAccessIterator first,
	 		   RandomAccessIterator last,
	 		   Comp cmp){
	typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
	if(last - first < 2) return;//没有元素或者只有1个元素直接返回。
	difference_type len = last - first;
	difference_type parent = (len - 2) / 2;

	//下面这段代码的问题在于，我们不知道parent的类型是不是有符号的，如果是无符号的，当parent == 0的时候，
	//--parent仍然是大于0的，所以不能这么写。

	// while(parent >= 0){
	// 	__adjust_heap(first, parent, len, *(first + parent), cmp);
	// 	--parent;
	// }
	
	while(true){
		__adjust_heap(first, parent, len, *(first + parent), cmp);
		if(parent == 0) return;
		--parent;
	}
	
}

}//namespace my_tiny_stl

#endif//__STL_HEAP_H