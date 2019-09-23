#ifndef __STL_ITERATOR_H
#define __STL_ITERATOR_H

namespace my_tiny_stl{

/**
 * 整个SGI STL的一个核心设计思路是将容器与算法两部分分开，然后通过迭代器将这两部分结合在一起。
 * 在算法中很可能会用到迭代器的相应类型，可以是创建一个迭代器相应类型的变量，常见的用法在于函数
 * 的重载，根据迭代器相应类型的不同来调用不同的函数。
 *
 * 如果迭代器通过类模板实现而不是原生指针的话，我们完全可以通过内嵌声明来定义迭代器的相应类型。
 * 比如：
 * template<class T>
 * class iterator{
 * public:
 * 		typedef T 	value_type;
 * 		typedef T* 	pointer;
 * 		typedef T& 	reference;;
 * };
 * 当我们要使用迭代器的相应类型的时候只需iterator<T>::value_type。这些看起来似乎都没有什么问题
 * 那么我们为什么还要有一个iterator_trait呢？似乎除了增加了一层间接性外，并没有给我们带来什么好处。
 * 真的是这样吗？不是！考虑原生指针，如果我们也需要原生指针的相应型别，因为原生指针并不是通过类实现的，
 * 因而也不能通过我们前面所讲的内嵌声明来解决问题。SGI STL就是通过一层间接性+类模板的特化来实现我们的目地。
 */

/**
 * 迭代器相应的类别有哪些呢？有5种，第一个是value_type，表示迭代器指向对象的类型。pointer和reference
 * 表示迭代器指向对象的指针类型与引用类型。difference_type表示两个迭代器之间的举例，通常都是ptrdiff_t
 * 定义在头文件<stddef.h>中。我们关注的是最后一种类型iterator_category，即迭代器的类型。
 *
 *什么是迭代器的类型呢？这个跟迭代器的操作有关。如果只能读则是input iterator类型，如果只能写则是output
 *iterator类型。如果可以读写，则是Forward iterator类型。前面这三种类型都是可以++操作的。除了Forwared iterator的特性外，
 *还能执行--操作，则是Bidirectional iterator类型。最后一种就是Random Access iterator，这种类型的迭代
 *器什么操作都可以执行。在SGI STL的中，是通过定义若干个空类来实现的，之所以为空原因是之作标记只用。这跟头文件
 *<new>中定义类型throw_t是一样的道理。这也是一种编程技巧。
 */

class input_iterator_tag{};
class output_iterator_tag{};
class forward_iterator_tag : public input_iterator_tag{};
class bidirectional_iterator_tag : public forward_iterator_tag{};
class random_access_iterator_tag : public bidirectional_iterator_tag{};

/**
 * 新的问题出现了，这里为什么要有公有继承。我们知道公有继承是is-a的关系，所以举例来说对于forward_iterator_tag
 * 类型的迭代器，也同时是一个input_iterator_tag的迭代器。这样的话，如果某个算法是针对于input_iterator_tag
 * 类型的迭代器开发的，那么forward_iterator_tag类型的迭代器同样也是用与这个算法。这就是我们继承的目的。
 *
 * 因为不同类型的迭代器有着不同的操作特性，而这些特性会带来效率上的不同，所以一个好的原则是，同一个算法对于更加具体的
 * 迭代器应该提供另外一种定义，使得效率能够最大化。一个典型的例子是copy算法，提过了一个针对random_access_iterato
 * r_tag更加高效率的版本。
 */


template<class T>
class iterator_traits{
public:
	typedef typename T::value_type 			value_type;
	typedef typename T::pointer 			pointer;
	typedef typename T::reference 			reference;
	typedef typename T::iterator_category	iterator_category;
	typedef typename T::difference_type		difference_type;
}; 

/**
 * 我们需要对原生指针进行偏特化。
 */

template<class T>
class iterator_traits<T*>{
public:
	typedef T 			value_type;
	typedef T*			pointer;
	typedef T& 			reference;
	typedef ptrdiff_t	difference_type;
	typedef random_access_iterator_tag iterator_category;
};

/**
 * 特化完成了吗？好象是。我们考虑一种情况，按照我们的特化，上述对于const T*，萃取出的value_type类型是
 * const T，我们本意是通过萃取出相应的类型声明一个局部变量或者临时变量，如果这个变量是const的，很可能
 * 不是我们希望的，所以我们还需要对const T*进行特化。  
 */

template<class T>
class iterator_traits<const T*>{
public:
	typedef T 			value_type;
	typedef const T*	pointer;
	typedef const T& 	reference;
	typedef ptrdiff_t	difference_type;
	typedef random_access_iterator_tag iterator_category;
};

/**
 * 所以为了能够与STL中的萃取相融，用户自己定义的迭代器，一定要内嵌声明迭代器对应的这5种类型。为了方便用户
 * SGI STL声明了一个类模板iterator，如果每个新设计的迭代器如果都继承这个类的话，便可以满足STL所需。 
 */

template<class T,
		 class Category,
		 class Pointer = T*,
		 class Reference = T&,
		 class Distance = ptrdiff_t>
class iterator{
public:
	typedef T 			value_type;
	typedef Category 	iterator_category;
	typedef Pointer 	pointer;
	typedef Reference 	reference;
	typedef Distance 	difference_type;
};

/**
 * 比如我们现在要定义一个迭代器，通过继承iterator即可满足STL要求。通常当一个迭代器设计好之后，它的
 * 类型也是随之确定的，由数据结构决定。
 * template<class T>
 * class my_iterator : public std::iterator<T, std::random_access_iterator_tag>{
 * 		...
 * };
 */


template<class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator& obj){
	return iterator_traits<Iterator>::iterator_category();
}

template<class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator& obj){
	return static_cast<iterator_category<Iterator>::difference_type*>(0);
}

template<class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator& obj){
	return static_cast<iterator_category<Iterator>::value_type*>(0);
}

}//my_tiny_stl
#endif //__STL_ITERATOR_H