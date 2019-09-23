#ifndef __MY_STL_ALLOC_H
#define __MY_STL_ALLOC_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/**
 * 对于内存的分配，最简单的方式就是使用operator new()或者malloc()来向系统heap申请内存即可。那么
 * STL中为什么又要存在一级、二级配置器这样的机制呢？
 * 原因在于，我们每一次向系统heap申请内存，除了申请的空间外，还有额外空间用来存储这块空间的说明信息，
 * 这块额外空间的大小是固定的，所以如果我们只是申请很小的一块内存，就会存在空间浪费问题，而且使用系统
 * 调用从heap分配内存，效率也比较低。STL的解决方法是先从系统heap申请一块大内存作为内存池，以后对于小
 * 规模的内存申请我们可以之间从这个内存池中取，效率会很高。 
 */


/**
 * 第一级配置器__malloc_alloc_template，第一级配置器就是对malloc()与free()的简单封装，我们这里
 * 模仿operator new()增加了异常处理机制。第一级配置器只关注按字节个数来分配内存，与分配对象类型无关。
 */

/**
 * 这里有一个很有意思的问题，为什么模板参数是int，这个inst是干嘛的？实际上，__malloc_alloc_template
 * 完全可以设计为一个类而不是类模板。但是为了模板编程考虑，我们把它设置为一个类模板，参数int只是用来实例化
 * 生成一个类。这也是一种模板编程技巧，请注意。
 * 
 */
namespace my_tiny_stl{

template<int inst>
class __malloc_alloc_template{
public:
	typedef void (*malloc_handler_t)();
private:
	static void* oom_allocate(size_t n);	
	static void* oom_reallocate(void* ptr, size_t n);
	static malloc_handler_t malloc_handler;
public:
	static void* allocate(size_t n){
		void* ret;
		if( !(ret = malloc(n)))
			return oom_allocate(n);
		return ret;
	}	

	static void* reallocate(void* ptr, size_t n){
		void* ret;
		if( !(ret = realloc(ptr, n)))
			return oom_reallocate(ptr, n);

		return ret;
	}

	static void delete(void* ptr){
		free(ptr);
	}

	static malloc_handler_t set_malloc_handler(set_malloc_handler new_handler){
		malloc_handler_t old_handler = malloc_handler;
		malloc_handler = new_handler;

		return old_handler;
	}
};
template<int inst>
malloc_handler_t __malloc_alloc_template<inst>::malloc_handler = 0;

template<int inst>
void* __malloc_alloc_template<inst>::oom_allocate(size_t n){
	void* result;

	for(;;){
		if(malloc_handler == 0)//可能在异常处理的时候，异常处理函数发生了变化，所以我们需要重新判断。
			throw bad_alloc();
		(*malloc_handler)();
		if( (result = malloc(n)))
			return result;
	}
}

template<int inst>
void* __malloc_alloc_template<inst>::oom_reallocate(void* ptr, size_t n){
	void* result;

	for(;;){
		if(malloc_handler == 0)//可能在异常处理的时候，异常处理函数发生了变化，所以我们需要重新判断。
			throw bad_alloc();
		(*malloc_handler)();
		if( (result = realloc(ptr, n)))
			return result;
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;

/**
 * 第二级配置器__default_alloc_template
 */

template<int inst>
class __default_alloc_template{
private:
	static const int ALIGN = 8;
	static const int MAX_BYTES = 128;
	static const int NFREELISTS = MAX_BYTES / ALIGN;
	static const int NUMS = 20;

private:
	struct obj{
		struct obj* next;
	};

private:
	static char* 	start_free;
	static char* 	end_free;
	static size_t 	heap_size;
	static obj* free_list[NFREELISTS];

private:
	static void* chunk_alloc(size_t n, int& objs);
	static void* refill(size_t n);
	static size_t ROUND_UP(size_t n){
		return (n + ALIGN - 1) & ~(ALIGN - 1);
	} 
	static size_t FREELIST_INDEX(size_t n){
		return (n + ALIGN - 1) / ALIGN - 1;
	}

public:
	static void* allocate(size_t n);
	static void* reallocate(void* ptr, size_t old_size, size_t new_size);
	static void  deallocate(void* ptr, size_t n);
};

template<int inst>
char* __default_alloc_template<inst>::start_free = 0;

template<int inst>
char* __default_alloc_template<inst>::end_free = 0;

template<int inst>
size_t __default_alloc_template<inst>::heap_size = 0;

template<int inst>
obj* __default_alloc_template<inst>::free_list = {0};

template<int inst>
void* __default_alloc_template<inst>::chunk_alloc(size_t per_size, int& objs){
	void* result;
	size_t total_bytes = per_size * objs;
	size_t bytes_left = end_free - start_free;
	obj** ptr_of_dest_list;
	size_t bytes_to_get;
	size_t size;

	if(bytes_left >= total_bytes){
		result = start_free;
		start_free += total_bytes;
		return result;
	}

	objs = bytes_left / per_size;
	if(objs > 0){
		result = start_free;
		start_free += objs * per_size;
		return result;
	}

	/**
	 * 到这一步，表示当前内存池中剩余的空间连一个区块大小都无法提供，所以我们需要将内存池剩余的空间
	 * 分配出去，同时申请新的内存。
	 */
	
	//把当前内存池中剩余的空间分配出去。
	if(bytes_left > 0){
		ptr_of_dest_list = free_list + FREELIST_INDEX(bytes_left);
		((obj*)start_free)->next = *ptr_of_dest_list;
		*ptr_of_dest_list = (obj*)start_free; 
	}

	/**
	 * 这里我们需要考虑一个问题，当需要重新分配空间的时候，应该分配多大的空间呢？SGI STL选择的是用户
	 * 需求的2倍。
	 */
	bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

	/**
	 * 尝试重新分配内存，如果分配内存成功的情况下，重新设置start_free、end_free、heap_size的值，
	 * 然后重新调用chunk_alloc即可，问题的难点在于当分配内存失败的时候，我们该如何处理呢？ SGI STL
	 * 的解决方式是从当前比用户申请的区块大的相应链表中去寻找空闲的区块分配给用户，如果还不能解决问题，
	 * 我们尝试使用一级配置器来分配内存，可能会抛出异常，也有可能情况会改善。
	 */

	if( !(start_free = malloc(bytes_to_get))){
		for(size = per_size; size <= MAX_BYTES; size += ALIGN){
			ptr_of_dest_list = free_list + FREELIST_INDEX(size);
			if(*ptr_of_dest_list){
				start_free = (char*)*ptr_of_dest_list;
				end_free = start_free + size;
				*ptr_of_dest_list = ((obj*)start_free)->next;

				return chunk_alloc(per_size, objs);
			}
		}
		/**
		 * 尝试调用一级配置器来解决问题。
		 */
		start_free = (char*)malloc_alloc::allocate(bytes_to_get);
	}

	end_free = start_free + bytes_to_get;
	heap_size += bytes_to_get;

	return chunk_alloc(per_size, objs);
}

template<int inst>
void* __default_alloc_template<inst>::refill(size_t n){
	int objs = NUMS;
	void * result = chunk_alloc(n, objs);
	obj** ptr_of_dest_list;
	obj* tmp;

	if(objs == 1) return result;
	
	ptr_of_dest_list = free_list + FREELIST_INDEX(n);
	*ptr_of_dest_list = ((obj*)result)->next;
	tmp = *ptr_of_dest_list;

	for(int i = 2; i < objs; ++i){
		tmp->next = (obj*)((char*)tmp + n);
		tmp = tmp->next;
	}
	tmp->next = (obj*)0;

	return result;
}

template<int inst>
void* __default_alloc_template<inst>::allocate(size_t n){
	obj** ptr_of_dest_list;
	void* ret;

	if(n > MAX_BYTES)
		return malloc_alloc::allocate(n);

	ptr_of_dest_list = free_list + FREELIST_INDEX(n);
	ret = (void*)(*ptr_of_dest_list);
	if(ret){
		*ptr_of_dest_list = ret->next;
		return ret;
	}
	
	return refill(ROUND_UP(n));
}

/**
 * reallocate也是一个比较有意思的函数。第一级配置器中的reallocate是直接调用系统函数realloc的
 * 但是这里需要多考虑一些内容。当新旧长度都大于MAX_BYTES的时候，我们选择直接调用系统函数realloc，
 * 这样效率会高一些。其实我们仍然可以通过调用二级配置器的allocate和deallocate来完成，但是在这种情况
 * 下相比于直接调用系统调用realloc效率会慢很多。
 */
template<int inst>
void* __default_alloc_template<inst>::reallocate(void* ptr, size_t old_size, size_t new_size){
	void* result;
	size_t min_size;

	if(old_size > MAX_BYTES && new_size > MAX_BYTES) return realloc(ptr, new_size);

	if(ROUND_UP(old_size) == ROUND_UP(new_size)) return ptr;

	result = allocate(new_size);
	min_size = old_size > new_size ? new_size : old_size;

	memcpy(result, ptr, min_size);
	deallocate(ptr, old_size);

	return result;
}

template<int inst>
void __default_alloc_template<inst>::deallocate(void* ptr, size_t n){
	obj** ptr_of_dest_list;

	if(n > MAX_BYTES)
		return malloc_alloc::deallocate(ptr);

	ptr_of_dest_list = free_list + FREELIST_INDEX(n);
	((obj*)ptr)->next = *ptr_of_dest_list;
	*ptr_of_dest_list = (obj*)ptr;
	
}

}//namespace my_tiny_stl
#endif //__MY_STL_ALLOC_H