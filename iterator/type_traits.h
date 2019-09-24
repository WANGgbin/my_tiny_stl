#ifndef __TYPE_TRAITS_H
#define __TYPE_TRAITS_H

/**
 * type_traits机制是干什么的呢？当涉及到对象的拷贝构造、赋值等操作的时候，根据能否直接进行内存的
 * 复制可以有不同的版本，而之所以考虑这个是为了尽量让程序更加高效率。这就是type_traits的用途。
 */
namespace my_tiny_stl{

/**
 * 只用作标记，所以是空类，这已经不是第一次见到了。注意这种编程技巧。
 */
class true_type{};
class false_type{};

/**
 * 默认对于所有类型都是false_type，这时一种保守的行为。
 */
template<class T>
class type_traits{
public:
	typedef false_type has_trivial_default_constructor;
	typedef false_type has_trivial_copy_constructor;
	typedef false_type has_trivial_assignment_operator;
	typedef false_type has_trivial_destructor;
	typedef falst_type is_POD_type; 
};

/**
 * 对于某个具体的类型，我们可以通过特化type_traits来指定各个内嵌类型。我们这里拿int来举例，其他类型
 * 同样的特化。
 */

tempalte<>
class type_traits<char>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<unsigned char>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<signed char>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<short>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<unsigned short>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<int>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<unsigned int>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<long int>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<unsigned long int>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<float>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};


tempalte<>
class type_traits<double>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<>
class type_traits<long double>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

tempalte<class T>
class type_traits<T*>{
public:
	typedef true_type has_trivial_default_constructor;
	typedef true_type has_trivial_copy_constructor;
	typedef true_type has_trivial_assignment_operator;
	typedef true_type has_trivial_destructor;
	typedef true_type is_POD_type;
};

}//my_tiny_stl

#endif//__TYPE_TRAITS_H