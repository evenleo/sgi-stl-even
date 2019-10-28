#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#define __SGI_STL_INTERNAL_CONSTRUCT_H

#include <new.h>  //欲使用 placement new，需先包含此文件

__STL_BEGIN_NAMESPACE

template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);  //placement new
}

//第一个版本，接收一个指针
template <class T>
inline void destroy(T* pointer)
{
    pointer->~T();
}

//如果有trivial destructor，什么也不做
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {
    //no_op
}

//如果没有trivial destructor，调用第一版本的destroy
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for ( ; first < last; ++first) 
        destroy(&*first);
}

//判断元素的数值型别(value_type)，是否有trivial destructor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

//第二个版本，就收两个迭代器，此函数设法找出元素的数值型别，利用type_traits求取最适当措施
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}

//以下是destroy第二版本对两个迭代器为char*和wchar_t*的特化版
//如果区间内的元素类型为char或wchar_t，则destroy什么也不做
inline void destroy(char*, char*) {  }
inline void destroy(wchar_t*, wchar_t*) {  }

__STL_END_NAMESPACE

#endif