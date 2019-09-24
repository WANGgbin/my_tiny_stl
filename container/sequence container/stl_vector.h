#ifndef __STL_VECTOR_H
#define __STL_VECTOR_H

#include <stddef.h>

namespace my_tiny_stl{

template<class T, class Alloc = alloc>
class vector{
public:
	typedef T value_type;
	typedef T* iterator;
	typedef T& reference;
	typedef ptrdiff_t difference_type;
	typedef simple_alloc<T, Alloc> data_allocator;

private:
	iterator start;
	iterator finish;
	iterator end_of_storage;

private:
	iterator allocate_and_fill(size_t n, const T& x){
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);

		return result;
	}

	void fill_initialize(size_t n, const T& x){
		start = allocate_and_fill(n, x);
		finish = start + n;
		end_of_storage = finish;
	}

	void insert_aux(iterator pos, const T& x);
public:
	vector(){start = 0; finish = 0; end_of_storage = 0;}
	vector(size_t n, const T& x){fill_initialize(n, x);}
	vector(int n, const T& x){fill_initialize(n, x);}
	vector(long int n, const T& x){fill_initialize(n, x);}
	explicit vector(size_t n){fill_initialize(n, T());}

	template<class Interator>
	vector(Interator first, Interator last){
		difference_type n = distance(first, last);
		start = data_allocator::allocate(n);
		finish = uninitialized_copy(first, last, start);
		end_of_storage = finish;		
	}

	~vector(){
		destroy(start, finish);
		data_allocator::deallocate(start, end_of_storage - start);
	}

	void insert(iterator pos, size_t n);
	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);
};

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, const T& x){
	difference_type n = end_of_storage - finish;
	if(n >= 1){
		if(pos == finish){
			construct(&*pos, x);
			finish += 1;
			return;
		}

		construct(&*finish, *(finish - 1));
		copy_backward(pos, finish - 1, finish);
		*pos = x;
		finish++;
		return;
	}

	/**
	 * 这里注意分配新空间的大小应该是多少？我们需要判断原来空间的大小是不是为0,如果为0,则分配1个大小
	 * 的空间，如果不为0,则需要分配原来空间大小的2倍。
	 */
	size_t old_size = end_of_storage - start;
	size_t new_size = old_size > 0 ? 2 * old_size : 1;

	iterator new_start = data_allocator::allocate(new_size);
	iterator new_pos;
	iterator new_finish;
	try{
		new_pos = uninitialized_copy(start, pos, new_start);
		construct(&*new_pos, x);
		new_finish = uninitialized_copy(pos, finish, new_pos + 1);
	}
	catch(...){
		data_allocator::deallocate(new_start, new_size);
		throw;
	}

	destroy(start, finish);
	data_allocator::deallocate(start, end_of_storage - start);

	start = new_start;
	finish = new_finish;
	end_of_storage = start + old_size;

}


template<class T, class Alloc>
void vector<T, Alloc>::insert(iterator pos, size_t n){
	difference_type left;
	iterator new_finish;

	if(n > 0){
		left = end_of_storage - finish;
		if(left >= n){
			 difference_type elements_after = finish - pos;
			 if(elements_after >= n){
			 	new_finish = uninitialized_copy(finish - n, finish, finish);
			 	copy_backward(pos, finish - n, finish);
			 	fill_n(pos, n, x);
			 	finish = new_finish;
			 }
			 else{
			 	difference_type len = n - elements_after;
			 	new_finish = uninitialized_copy(pos, finish, finish + len);
			 	uninitialized_fill_n(finish, len, x);
			 	fill_n(pos, elements_after, x);
			 	finish = new_finish;
			 }
		}
		else{
			/**
			 * 这里我们考虑的第一个问题是，新分配的空间大小是多少？是原来空间大小的2倍吗？
			 * 非也！这里大小还跟插入的元素个数有关。我们取 new_size = old_size + 
			 * max(old_size, n);
			 */
			size_t old_size = end_of_storage - start;
			size_t new_size = old_size + (old_size > n ? old_size : n);

			new_start = data_allocator::allocate(new_size);
			iterator tmp;
			try{
				tmp = uninitialized_copy(start, pos, new_start);
				tmp = uninitialized_fill_n(tmp, n, x);
				tmp = uninitialized_copy(pos, finish, tmp);
			}
			catch(...){
				data_allocator::deallocate(new_start, new_size);
				throw;
			}

			destroy(start, finish);
			data_allocator::deallocate(start, old_size);

			start = new_start;
			finish = tmp;
			end_of_storage = new_start + new_size;
		}
	}
}

template<class T, class Alloc>
iterator vector<T, Alloc>::erase(iterator pos){
	if(pos != (finish - 1))
		copy(pos + 1, finish, pos);
	destroy(&*(finish - 1));
	--finish;

	return pos;
}

template<class T, class Alloc>
iterator vector<T, Alloc>::erase(iterator first, iterator last){
	iterator tmp = copy(last, finish, first);
	destroy(tmp, finish);
	finsih -= (last - first);
	return first;
}

}
#endif//__STL_VECTOR_H