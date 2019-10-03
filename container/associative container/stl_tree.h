#ifndef __STL_TREE_H
#define __STL_TREE_H

namespace my_tiny_stl{

typedef bool color_type;
const color_type red = true;
const color_typpe black = false;


template<class T1, class T2>
struct pair{
	typedef T1 first_type;
	typedef T2 second_type;

	first_type first;
	second_type second;

	pair(){}
	pair(const first_type& obj1, const second_type& obj2) : first(obj1), second(obj2){}
};

/**
 * ******************************节点部分*******************************************
 */
struct rb_tree_base_node{
	typedef rb_tree_base_node* base_ptr;

	color_type color;
	base_ptr parent;
	base_ptr left;
	base_ptr right;

	static base_ptr minimum(base_ptr x){
		while(x)
			x = x->left;
		return x;
	}

	static base_ptr maxmum(base_ptr x){
		while(x)
			x = x->right;
		return x;
	}
};

template<class T>
struct rb_tree_node : public rb_tree_base_node{
	T data;
};

/**
 * ********************************迭代器部分**************************************
 */

struct rb_tree_base_iterator{
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t 	difference_type;
	typedef rb_tree_base_node*	base_ptr;

	base_ptr node;

	rb_tree_base_iterator(base_ptr par = 0){node = par;}
	void increment();
	void decrement();
};

/**
 * 最大节点的写一个位置是end()，即根节点的父节点。
 */
void rb_tree_base_iterator::increment(){
	/**
	 * 如果有右子树的话，右子树最左边的节点就是当前节点的后继节点。
	 */
	if(node->right){
		node = node->right;
		while(node->left)
			node = node->left;	
	}
	/**
	 * 如果没有右子树，需要往上走。寻找的节点满足：该节点的左子树的最右节点为当前节点。
	 * （除了当前节点为最大节点的情况）
	 */
	else{
		base_ptr parent = node->parent;
		while(parent->right == node){
			node = parent;
			parent = node->parent;
		}
	}

	/**
	 * 这一步是什么意思呢？处理的特殊情况是当根节点为最大节点即根节点无右子树的时候，求根节点的下一个节点。
	 * 通过上一步的循环，此时parent是根节点，node是根节点的父节点。最大节点的下一个节点应该是end()，
	 * 所以此时不能将parent赋值给node，否则父节点的下一个节点是自己，这显然是不对的。一定要注意这种特殊情况。
	 */
	if(node->right != parent)
		node = parent;	
}

void rb_tree_base_iterator::decrement(){

	/**
	 * 考虑的特殊情况是当当前节点是根节点的父节点的时候，注意红黑树中满足node->parent->parent==node的
	 * 节点有两个，一个是根节点一个是根节点的父节点，所以千万不要忘记条件node->color==red.
	 */
	
	if(node->color == red && node->parent->parent == node)
		node = node->right;
	else if(node->left){
		/**
		 * 如果存在左子树，则左子树的最右边的节点为当前节点的下一个节点。
		 */
		node = node->left;
		while(node->right)
			node = node->right;
	}
	else{
		/**
		 * 如果不存在左子树，向上寻找这样的节点，当前节点为该节点右子树的最左节点。
		 */
		base_ptr parent = node->parent;
		while(parent->right == node){
			node = parent;
			parent = node->parent;
		}
		node = parent;
	}
}

inline bool operator==(const rb_tree_base_iterator& obj1,
					   const rb_tree_base_iterator& obj2){
	return obj1.node == obj2.node;
}

inline bool operator!=(const rb_tree_base_iterator& obj1,
					   const rb_tree_base_iterator& obj2){
	return obj1.node != obj2.node;
}

template<class T>
struct rb_tree_iterator : public rb_tree_base_iterator{
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef rb_tree_iterator<T> self;
	typedef rb_tree_node* link_type;

	rb_tree_iterator(){}
	explicit rb_tree_iterator(link_type par) : rb_tree_base_iterator(par){}


	self& operator++(){
		node.increment();
		return *this;
	}

	self operator++(int){
		self ret = *this;
		++*this;
		return ret;
	}

	self& operator--(){
		node.increment();
		return *this;
	}

	self operator--(int){
		self ret = *this;
		--*this;
		return ret;
	}

	reference operator*(){
		return ((link_type)node)->data;
	}

	pointer operator->(){
		return &(operator*());
	}
};

template<class Key, class Value, class KeyOfValue, class Comp, class Alloc = alloc>
class rb_tree{
public:
	typedef rb_tree_node<Value>	rb_tree_node;
	typedef rb_tree_node* link_type;
	typedef simple_alloc<rb_tree_node> node_allocator;
	typedef size_t size_type;
	typedef rb_tree_iterator<Value>	iterator;
	typedef rb_tree_base_node::base_ptr base_ptr;
	typedef rb_tree<Key, Value, KeyOfValue, Comp, Alloc>	self;
private:
	size_type node_count;
	Comp comp;

	/**
	 * 为什么要引入header呢？在树的各种操作中，最需要注意的就是边界情况的发生，通常就是当走到根节点
	 * 的时候需要特殊的处理。SGI STL为了尽量消除根节点的特殊性，为根节点提供了一个父节点即header.
	 */
	link_type header;

private:
	link_type get_node(){return node_allocator::allocate();}
	void put_node(link_type ptr){node_allocator::deallocate(ptr);}
	link_type create_node(const Value& x){
		link_type ret = get_node();
		try{
			construct(&ret->data, x);
		}
		catch(...){
			destroy(&ret->data);
			throw;
		}
		return ret;
	}

	link_type clone_node(link_type ptr){
		link_type ret = create_node(ptr->data);

		ret->right = 0;
		ret->left = 0;

		return ret;
	}

	void destroy_node(link_type ptr){
		destroy(&ptr->data);
		put_node(ptr);
	}

	link_type& root() const {return (link_type&)/*这里类型转化只能是link_type&而不能是link_type*/(header->parent);}
	link_type& leftmost() const {return (link_type&)(header->left);}
	link_type& rightmost() const {return (link_type&)(header->right);}

	static link_type& parent(link_type x){return (link_type&)(x->parent);}
	static link_type& left(link_type x){return (link_type&)(x->left);}
	static link_type& right(link_type x){return (link_type&)(x->right);}
	static Value&	value(link_type x){return (x->data);}
	//这里注意key函数的返回类型，因为我们是不希望改变key值的，所以返回类型是const的，至于
	//函数为什么返回引用类型，因为这里的key并不是产生的局部对象，而是value的部分内容。
	static const Key& key(link_type x){return KeyOfValue()(value(x));}
	static color_type& color(link_type x){return x->color;}

	static link_type& parent(base_ptr x){return (link_type&)(x->parent);}
	static link_type& left(base_ptr x){return (link_type&)(x->left);}
	static link_type& right(base_ptr x){return (link_type&)(x->right);}
	static Value&	value(base_ptr x){return (((link_type)x)->data);}
	//这里注意key函数的返回类型，因为我们是不希望改变key值的，所以返回类型是const的，至于
	//函数为什么返回引用类型，因为这里的key并不是产生的局部对象，而是value的部分内容。
	static const Key& key(base_ptr x){return KeyOfValue()(value(x));}
	static color_type& color(base_ptr x){return ((link_type)x)->color;}	

	static link_type minimum(link_type x){
		return (link_type)(rb_tree_base_node::minimum(x));
	}

	static link_type maxmum(link_type x){
		return (link_type)(rb_tree_base_node::maxmum(x));
	}

	void init(){
		header = get_node();
		color(header) = red;
		root() = 0;
		leftmost() = header;
		rightmost() = header;
	}

private:
	/**
	 * 这部分重点介绍rb-tree的插入操作.
	 */
	
	/**
	 * [rb_tree_rotate_left 该函数是红黑树的左旋转操作，整个操作中涉及到3对6个指针的变化，注意指针的变化
	 * 顺序即可，我们把x与y之间的互相指向放到最后，其他两对指针的变化顺序任意，为什么呢？因为x与y的指向会
	 * 改变x->parent与y->left的值，而其他两对指针的变化会引用x->parent与y-left。]
	 * @param x    [x是旋转点]
	 * @param root [root是header->parent，因为这里可能会改变header->parent的值，所以传递的是引用]
	 */
	void rb_tree_rotate_left(base_ptr x, base_ptr& root){
		base_ptr y = x->right;

		x->right = y->left;

		if(y->left)
			y->left->parent = x;

		y->parent = x->parent;

		if(root == x)
			root = y;
		else if(x->parent->left == x)
			x->parent->left = y;
		else
			x->parent->right = y;

		x->parent = y;
		y->left = x;
	}

	/**
	 * [rb_tree_rotate_right 该函数是红黑树的左旋转操作，整个操作中涉及到3对6个指针的变化，注意指针的变化
	 * 顺序即可，我们把x与y之间的互相指向放到最后，其他两对指针的变化顺序任意]
	 * @param x    [x是旋转点]
	 * @param root [root是header->parent，因为这里可能会改变header->parent的值，所以传递的是引用]
	 */
	void rb_tree_rotate_right(base_ptr x, base_ptr& root){
		base_ptr y = x->left;

		x->left = y->right;

		if(y->right)
			y->right->parent = x;

		y->parent = x->parent;

		if(root == x)
			root = y;
		else if(x->parent->left == x)
			x->parent->left = y;
		else
			x->parent->right = y;

		x->parent = y;
		y->right = x;
	}

/**
 * 那么什么时候，我们需要调整红黑树呢？
当插入的节点不是根节点且其父节点为红节点的时候。如果插入节点是根节点只需要修改节点颜色即可。
我们分三种情况讨论：
1、当父节点的兄弟节点是红节点的时候，这个时候，我们需要将父节点与伯父节点的颜色调整为黑色，同时将祖父节点的颜色调整为红色，然后从祖父节点的角度出发，继续判断是否调整红黑树。这里为什么还需要继续向上调整呢？
因为原来祖父节点是黑色的，所以祖父节点的父节点的颜色我们是不能确定的，现在我们将祖父节点调整为红色，由红黑树的特性我们知道，红节点的父节点必须是黑节点。所以，这个时候我们需要继续调整。

2、当伯父节点不存在或者是黑色的时候，我们又需要分两种情况讨论：
（1）、当插入节点是父节点的右节点的时候（前提：父节点是祖父节点的左孩子），我们先需要以父节点为旋转点左旋一次，然后修改插入节点颜色为黑色，祖父节点颜色为红色同时以祖父节点为旋转点右旋一次。
（2）、当插入点是父节点的左节点的时候，我们需要修改父节点颜色为黑色，祖父节点颜色为红色同时以祖父节点为旋转点右旋一次。

以上三种情况是以，父节点是祖父节点的左孩子举例的，当父节点是祖父节点的右节点的时候，是同样的道理。

调整红黑数的伪代码为：

while(需要调整){
	if(父节点是祖父节点的左孩子){
		if(伯父节点存在而且是红色的){
			调整父节点和伯父节点的颜色为黑色。
			调整祖父节点的颜色为红色。
			继续向上调整。
		}
		else{//伯父节点不存在或者是黑色的
			if(插入节点是父节点的右孩子){
				以父节点为旋转节点左旋。
			}
			
			调整父节点颜色为黑色。
			调整祖父节点的颜色为红色。

			以祖父节点为旋转点右转。
		}
	}
	else{//父节点是祖父节点的右孩子

		if(伯父节点存在而且是红色的){
			调整父节点和伯父节点的颜色为黑色。
			调整祖父节点的颜色为红色。
			继续向上调整。
		}
		else{//伯父节点不存在或者是黑色的
			if(插入节点是父节点的左孩子){
				以父节点为旋转节点右旋。
			}
			
			调整父节点颜色为黑色。
			调整祖父节点的颜色为红色。

			以祖父节点为旋转点左转。
		}
	
	}
	让根节点为黑节点。//因为调整过程中可能会把根节点调整为红色，最后我们需要修正回来。
}
 */

	void rb_tree_rebalance(base_ptr x, base_ptr& root){

		x->color = red;
		if(x->parent->parent->left = x->parent){
			while(!(x == root || x->parent->color == black)){
				y = x->parent->parent->right;
				if(y && y->color == red){
					x->parent->color = black;
					y->color = black;
					x->parent->parent->color = red;
					x = x->parent->parent;
				}
				else{
					if(x->parent->right == x){
						x = x->parent;
						rb_tree_rotate_left(x, root);
					}

					x->parent->color = black;
					x->parent->parent->color = red;

					rb_tree_rotate_right(x->parent->parent, root);
				}
			}
		}
		else{
			while(!(x == root || x->parent->color == black)){
				y = x->parent->parent->left;

				if(y && y->color == red){
					x->parent->color = balck;
					y->color = black;
					x->parent->parent->color = red;
					x = x->parent->parent;
				}
				else{
					if(x->parent->left == x){
						x = x->parent;
						rb_tree_rotate_right(x, root);
					}

					x->parent->color = black;
					x->parent->parent->color = red;

					rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}

		root->color = black;
	}

	iterator _insert(base_ptr x, base_ptr y, const Value& x);
	/**
	 * [_erase 函数删除以x为根节点的树]
	 * @param x [要删除树的根节点]
	 */
	void _erase(link_type x);

	rb_tree_base_node*
	_rb_tree_rebalance_for_erase(rb_tree_base_node* z,
								 rb_tree_base_node*& root,
								 rb_tree_base_node*& leftmost,
								 rb_tree_base_node*& rightmost);
public:

	pair<iterator, bool> insert_unqiue(const Value& x); 
	iterator insert_equal(const Value&  x);
	iterator find(const Key& k);
	void clear(){
		_erase(root());
		leftmost() = header;
		rightmost() = header;
		root() = 0;
		node_count = 0;
	}
	void erase(iterator pos){
		link_type y = (link_type)_rb_tree_rebalance_for_erase();
		destroy_node(y);
		--node_count;
	}
	/**
	 * 还需要做的是红黑数的copy操作。
	 */
	rb_tree(const Comp& comp_par = Comp()) : node_count(0), comp(comp_par){init();}
	rb_tree(const self& par){
		//todo
	}
	self& operator=(const self& par){
		//todo
		return *this;
	}

	~rb_tree(){clear();}
	iterator lower_bound(const Key& x);
	iterator upper_bound(const Key& x);
	size_type count(const Key& x);

};


/**
 * 这里面特别需要注意一点函数的返回类型为什么不能直接写成iterator。规则是当在类外面定义函数的时候
 * 函数的返回类型如果不是类模板的参数类型，则类型寻找是在类外寻找的，所以如果函数返回的是类中定义的
 * 类型则返回值一定要用关键字typename，然而函数的参数缺没有这样的限制，因为函数参数作用域是类作用域，
 * 可以看到类中定义的类型。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc = alloc>
typename rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::_insert(base_ptr x, base_ptr y, const Value& x){
	
	link_type new_node = create_node(x);
	if(y == header){
		root() = new_node;
		leftmost() = new_node;
		rightmost() = new_node;
	}
	else(!x || comp(KeyOfValue()(x), key(y))){
		left(y) = new_node;
		if(leftmost() == y)
			leftmost() = x;
	}
	else{
		right(y) = new_node;
		if(rightmost() == y)
			rightmost() = x;
	}

	parent(new_node) = y;
	left(new_node) = 0;
	right(new_node) = 0;

	++node_coutn;
	rb_tree_rebalance(new_node, header->parent);

	return iteartor(new_node);
}

template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
pair<rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::insert_unqiue(const Value& x){
	link_type parent = header;
	link_type cur = root();
	iterator tmp;

	/**
	 * 这部分循环的目的是寻找最终插入位置。
	 */
	while(cur){
		parent = cur;
		cur = comp(KeyOfValue()(x), key(cur)) ? left(cur) : right(cur); 
	}

	/**
	 * 最终插入位置可以分两类讨论，一中是插入位置是其父节点的左孩子，另一种是插入位置的右孩子
	 * 那么这两种有什么区别吗？
	 *
	 * （1）、如果二叉搜索树存在与该节点相等的非叶节点，那么待插入节点一定是被插入到该非叶节点右子树的最左侧的。
	 * 因为从该非叶节点角度看，其右侧的节点都比它大，而插入的重复节点又是比他大的节点中最小的，所以该插入节点
	 * 的最终插入位置一定是在该非叶节点的右子树的最左侧。
	 * （2）、如果该插入节点与某个叶节点重复，则一定是位于叶节点的右侧的。
	 * 
	 * 所以，我们可以先判断插入节点的插入位置，如果是其父节点的右孩子，则只需判断与父节点是否相同既可以判断是否重复。
	 * 如果插入节点的最终位置是在左侧则需要判断插入节点的值是否与其父节点前驱节点(插入之前)的值是否相同。
	 */
	tmp = iterator(parent);
	if(comp(KeyOfValue()(x), key(parent))){
		/**
		 * 一定不要忘记这里的分类讨论。如果小于，说明是其父节点的左孩子，因为如果tmp是begin，则其没有前驱。
		 */
		if(tmp == begin())
			return pair<iterator, bool>(_insert(cur, parent, x), true);
		--tmp;
	}

	if(comp(key(tmp.node), KeyOfValue()(x)))
		return pair<iterator, bool>(_insert(cur, parent, x), true);

	return pair<iterator, bool>(tmp, false);
}

template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::insert_equal(const Value& x){
	link_type parent = header;
	link_type cur = root();

	while(cur){
		parent = cur;
		cur = comp(key(cur), KeyOfValue()(x)) ? right(cur) : left(cur);
	}

	return _insert(cur, parent, x);
}

/**
 * 思路：寻找k的后继，如果后继为end()或者后继与k不相等则函数返回end()。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::find(const Key& k){
	link_type after = header;
	link_type cur = root();

	/**
	 * 这是查找后继的典型操作。同样我们也可以查找前驱。
	 */
	while(cur){
		if(!comp(key(cur), k)){
			after = cur;
			cur = left(cur);
		}
		else
			cur = right(cur);
	}

	/**
	 * while(cur){
	 * 		if(comp(key(cur), k)){
	 * 			before = cur;
	 * 			cur = right(cur);
	 * 		}
	 * 		else
	 * 			cur = left(cur);
	 * }
	 *
	 * iterator j = iterator(before);
	 * j有可能等于end()也可能为前驱。
	 */
	iterator j = iterator(after);

	return (after == end() || comp(k, key(j.node))) ? end() : j;
}

/**
 * 函数的实现中使用了递归，删除以x为根节点的子树，只有clear中调用这个函数。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::_erase(link_type x){
	while(x){
		_erase(right(x));
		link_type left = left(x);
		destroy_node(x);
		x = left;
	}
}

/**
 * 这部分内容是整个rb-tree中最复杂的部分，删除某个节点，难点在于删除后的调整。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree_base_node*
rb_tree<Key, Value, KeyOfValue, Comp, Alloc::
_rb_tree_rebalance_for_erase(rb_tree_base_node* z,
							 rb_tree_base_node*& root,
							 rb_tree_base_node*& leftmost,
							 rb_tree_base_node*& rightmost){
	//todo
}


/**
 * 这个函数的作用是返回指向第一个大于等于(不小于)k的节点的迭代器
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
void rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::lower_bound(const Key& k){
	link_type after = header;
	link_type cur = root();
	iterator j;
	while(cur){
		if(!(comp(key(cur), k))){
			j = iterator(cur);
			cur = left(cur);
		}
		else
			cur = right(cur);
	}

	return j;
}

/**
 * 这个函数的作用是返回指向第一个大于k的节点的迭代器，从描述上来看lower_bound与upper_bound
 * 似乎很相似，但实际上差很多。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
void rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::upper_bound(const Key& k){
	link_type after = header;
	link_type cur = root();
	iterator j;
	while(cur){
		if((comp(k, key(cur)))){
			j = iterator(cur);
			cur = left(cur);
		}
		else
			cur = right(cur);
	}

	return j;
}

/**
 * 红黑树计算k的个数的思路是，先通过lower_bound计算第一个不小于k的节点的迭代器，
 * 然后通过upper_bound计算第一个大于k的节点的迭代器，然后通过distance来计算这
 * 两个迭代器之间的举例。一定要注意lower_bound与upper_bound之间的区别。
 */
template<class Key, class Value, class KeyOfValue, class Comp, class Alloc>
rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::size_type
void rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::count(const Key& k){
	iterator first = lower_bound(k);
	iterator last = upper_bound(k);
	size_type ret;

	ret = distance(first, last);
	return ret;
}

}//namespace my_tiny_stl

#endif//__STL_TREE_H