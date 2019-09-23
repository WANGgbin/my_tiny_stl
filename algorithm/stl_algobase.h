/**
 * 该头文件通过函数模板实现了大多数常用的基本算法。
 */

#ifndef __STL_ALGOBASE_H
#define __STL_ALGOBASE_H

#include <string.h>

#include "../iterator/stl_iterator.h"
#include "../iterator/type_traits.h"

namespace my_tiny_stl{
/**
 * 我们强调，SGI STL是一个无所不用其及来追求效率的艺术品，一个很典型的例子就是基础算法copy的实现。
 * copy的底层实现就是通过赋值来完成的，但是有些对象是拥有trivial assignment operator的，也就是
 * 说这些对象的赋值操作本质上而言就是内存的拷贝，如果我们能够使用系统调用memmove、memcpy来完成拷贝
 * 任务的话，效率就会高很多。
 */


/**
 * 我们这里对c++中的引用再进行一个讨论。本质上将引用就是指针，是对指针的封装，使用起来更加安全。在c++
 * 中我们尽量使用引用，这时一种好的编程行为，除非你能对你的指针理解足够自信。我们的问题是传递参数的时候
 * 什么时候传递引用呢？传递引用有两个目的就跟c中的传递指针一样，第一个目的是效率较高，第二个目的是函数内部
 * 能够对相应的对象内容进行修改。通常，如果我们只是追求效率而不对对象内容进行修改的话，那么函数参数类型
 * 就可以是const type&。如果要对目标进行修改，可以是type&。对于第一种情况，任何情况下传递引用效率都是
 * 最好的吗？非也。什么情况下传递引用的效率竟然会低呢？当函数内部需要对象的拷贝的时候。如果传递引用，消耗有
 * 两步，第一步是指针的传递，第二步是对象的生成。而如果在传递参数的时候直接传递对象，则只有一步消耗。这就是
 * 为什么我们的copy实现传递不是引用。
 */

/**
 * 算法中另一个让我们迷惑的地放在于，拿copy举例，既然这个算法可以接受任意类型的迭代器，将参数名称命名为
 * Interator显得不是那么合理。单纯从名称的角度考虑，可以将参数命名为任何名称，这里命名为Interator的
 * 目的在于，告诉用户这个算法能接受的最低阶迭代器类型是InputIterator类型。这时STL的一个命名规则，这
 * 实际上是一个好的编程习惯，在模板编程中，参数的名称即可告诉用户，这个函数模板/类模板期待接受什么样的类型。
 */
template<class Interator, class Outerator>
Outerator copy(Interator first,
			   Interator last,
			   Outerator result){
	return _copy(first, last, result);
}

template<class Interator, class Outerator>
Outerator _copy(Interator first,
				Interator last,
				Outerator result){

}

template<>
char* _copy<char*>(char* first, char* last, char* result){
	memmove(result, first, last - first);
	return result + (last - first);
}

wchar_t* _copy(wchar_t* first, wchar_t* last, wchar_t* result){
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

template<class Interator, class Outerator>
class __copy{

};

template<class T>
class __copy<T*, T*>{
	
};

}//namespace my_tiny_stl  
#endif //__STL_ALGOBASE_h