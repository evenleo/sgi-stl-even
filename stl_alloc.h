#ifndef __SGI_STL_INTERNAL_ALLOC_H
#define __SGI_STL_INTERNAL_ALLOC_H

#include <iostream>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define __NODE_ALLOCATOR_THREADS false
#define __THROW_BAD_ALLOC std::cerr << "out of memoty" << std::endl; exit(1)

__STL_BEGIN_NAMESPACE

//一级配置器，直接调用malloc和free函数申请和释放内存
template<int inst>
class __malloc_alloc_template {
private:
    //oom = out of memroy，当内存不足时，使用下面这两个函数
    static void *oom_malloc(size_t);
    static void *oom_realloc(void*, size_t);
    static void (*__malloc_alloc_oom_handler)();

public:
    static void *allocate(size_t n) {
        void *result = malloc(n);
        if (0 == result)  //如果不足，用下面处理方法
            result = oom_malloc(n);
        return result;
    }
    
    //直接释放
    static void deallocate(void* p, size_t /*n*/)
    {
        free(p);
    }

    static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
        void *result = realloc(p, new_sz);
        if (0 == result)  //如果不足，用下面处理方法
            result = oom_realloc(p, new_sz);
        return result;
    }

    static void(*__set_malloc_handler(void(*f)))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

template<int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

//malloc内存不够的处理函数
template<int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void(*my_malloc_handler)();
    void *result;
    for (;;)
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler)
        {
            __THROW_BAD_ALLOC;   //如果没有设置处理函数，抛出异常退出
        }
        (*my_malloc_handler)();  //设置了处理函数，调用之，这里推测应该是回收一些内存的操作
        result = malloc(n);      //重新调用malloc，申请空间成功则返回地址，否则继续本循环
        if (result)
            return result;
    }
}

//realloc内存不够的处理函数，处理过程和上面函数类似，不注释了
template<int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (*my_malloc_handler)();
    void *result;
    for (;;)
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler)
        {
            __THROW_BAD_ALLOC;
        }
        result = realloc(p, n);
        if (result)
            return result;
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;  //一级配置器全局变量

template<class T, class Alloc>
class simple_alloc {
public:
    static T *allocate(size_t n)
                { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }
    static T *allocate(void)
                { return (T*)Alloc::allocate(sizeof(T)); }
    static void deallocate(T *p, size_t n)
                { if(0 != n) Alloc::deallocate(p, n * sizeof(T)); }
    static void deallocate(T *p)
                { Alloc::deallocate(p, sizeof(T)); }
};

//二级配置器，申请空间大于128byte使用一级配置器，否则使用二级配置器
template<bool threads, int inst>
class __default_alloc_template {
private:
	enum { ALIGN = 8 };                       //区间间隔字节数
	enum { MAX_BYTES = 128 };                 //最大字节数
	enum { NFREELISTS = MAX_BYTES / ALIGN };  //自由链表数组的长度

	static size_t ROUND_UP(size_t bytes) {    //向上取8的倍数
		return ((bytes + (size_t)ALIGN - 1) & ~(ALIGN - 1));
	}

	static size_t FREELIST_INDEX(size_t bytes) {  //获取所在自由链表数组的索引
		return ((bytes + (size_t)ALIGN - 1) / (size_t)ALIGN - 1);
	}

	union obj {
		union obj *free_list_link;
		char client_data[1];
	};

	static obj* volatile freeLists[NFREELISTS];

	static char *chunk_alloc(size_t size, int& nobj);  //配置size*nobjs大小的区块
	static void *refill(size_t __n);  //补充链表并返回一个有效的期望区块

	static char *start_free;  //内存池起始位置，只在chunk_alloc()中变化
	static char *end_free;    //内存池结束位置，只在chunk_alloc()中变化
	static size_t heap_size;

public:
    
    //配置空间
	static void *allocate(size_t n) {
		obj *volatile *my_free_list;
		obj *result;

		if (n > size_t(MAX_BYTES)) {  //大于128bytes，调用第一级配置器allocate()
			return malloc_alloc::allocate(n);
		}

		my_free_list = freeLists + FREELIST_INDEX(n);  //定位到对应自由链表
		result = *my_free_list;  //取当前链表的头结点
		if (result == 0) {       //如果取值为空
			void *r = refill(ROUND_UP(n));  //调用refill()函数
			return r;
		}
        
        //如果取值不为空，则自由链表指向下一个节点
		*my_free_list = result->free_list_link;  
		return result;
	}
    
    //释放空间
	static void deallocate(void* p, size_t n) {
		obj *q = (obj*)p;  //q指向待释放的区块
		obj *volatile *my_free_list;

		if (n > size_t(MAX_BYTES)) {  //大于128bytes，调用第一级配置的deallocate()
			malloc_alloc::deallocate(p, n);
			return;
		}

		my_free_list = freeLists + FREELIST_INDEX(n);  //定位到对应自由链表
		q->free_list_link = *my_free_list;  //待释放区块节点指向原自由链表的头结点
		*my_free_list = q;  //自由链表的头结点指向q，上述过程既是头插法插入q节点
	}

	static void *reallocate(void *p, size_t old_sz, size_t new_sz);
};

typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;  //二级配置器全局变量
typedef __default_alloc_template<false, 0> single_client_alloc;

//配置size*nobjs大小的区块，只被refill()函数调用
template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
    char* result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;

    if (bytes_left >= total_bytes) {  //如果内存池剩余空间大小大于等于total_bytes
        result = start_free;          //直接取空间并返回
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {  //如果内存池剩余空间[size, total_bytes)
        nobjs = bytes_left/size;      //取一个及以上区块，尽量多
        total_bytes = size * nobjs;   //然后取空间并返回
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        if (bytes_left > 0) {  //内存里还有些零头，配置给适当的free-list中
            obj* volatile* my_free_list = freeLists + FREELIST_INDEX(bytes_left);
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*)start_free;
        }
        //配置heap空间，用来补充内存池
        start_free = (char*)malloc(bytes_to_get);
        if (0 == start_free) {  //heap空间不足，malloc失败
            int i;
            obj *volatile *my_free_list, *p;
            /*系统实在没有内存了，向上释放链表已用内存*/
            for (i = size; i <= MAX_BYTES; i += ALIGN) {
                my_free_list = freeLists + FREELIST_INDEX(i);
                p = *my_free_list;
                if (0 != p) {
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }
            }
            end_free = 0;  //到这里已经到山穷水尽，调用第一级配置器
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            //这会导致抛出异常或改善内存不足的情况
        }
        //内存池补充足够，递归调用chunk_alloc()
        heap_size += bytes_left;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    }
}

template<bool threads, int inst>
void *__default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    char *chunk = chunk_alloc(n, nobjs);  //获取20*n的区块，nobjs为引用会被chunk_alloc改变
    obj* volatile *my_free_list;
    obj* result;
    obj* current_obj, *next_obj;
    int i;

    if (1 == nobjs) return chunk;  //如果只有一块，直接返回
    my_free_list = freeLists + FREELIST_INDEX(n);
    result = (obj*)chunk;  //取第一个块size大小的空间
    *my_free_list = next_obj = (obj*)(chunk + n);  //指向第二块size空间
    for (i = 1; ; i++) {  //循环将nobjs-1个size大小的空间依次插入到链表中
        current_obj = next_obj;
        next_obj = (obj*)((char*)next_obj + n);
        if (nobjs - 1 == i) {  //此时已经将nbojs-1个size大小的空间插入完毕，退出循环
            current_obj->free_list_link = 0;  
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

template<bool threads, int inst>
void *__default_alloc_template<threads, inst>::reallocate(void *p, 
                                                          size_t old_sz, 
                                                          size_t new_sz)
{
    void *result;
    size_t copy_sz;

    if (old_sz > (size_t)MAX_BYTES && new_sz > (size_t)MAX_BYTES) {
        return realloc(p, new_sz);  //直接调用realloc()函数
    }
    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;  //旧空间大小等于新空间大小，直接返回p
    result = allocate(new_sz);  //调用allocate配置新空间
    copy_sz = new_sz > old_sz ? old_sz : new_sz;  //取新旧空间大小的最小值
    memcpy(result, p, copy_sz);  //复制copy_sz大小的内容到新空间
    deallocate(p, old_sz);  //释放旧空间
    return result;
}

template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj *volatile 
__default_alloc_template<threads, inst>::freeLists[
    __default_alloc_template<threads, inst>::NFREELISTS] 
    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

__STL_END_NAMESPACE

#endif