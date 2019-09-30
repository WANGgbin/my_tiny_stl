#ifndef __STL_TREE_H
#define __STL_TREE_H

namespace my_tiny_stl{

typedef bool color_type;
const color_type red = true;
const color_typpe black = false;

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
	rb_tree_iterator(link_type par) : rb_tree_base_iterator(par){}


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
private:
	size_type node_count;
	Comp comp;
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

};
}//namespace my_tiny_stl

#endif//__STL_TREE_H