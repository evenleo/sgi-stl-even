#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#define __SGI_STL_INTERNAL_CONSTRUCT_H

#include <new.h>

__STL_BEGIN_NAMESPACE

template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);
}

template <class T>
inline void destroy(T* pointer)
{
    pointer->~T();
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {
    //no_op
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for ( ; first < last; ++first) 
        destroy(&*first);
}

inline void destroy(char*, char*) {  }
inline void destroy(wchar_t*, wchar_t*) {  }

template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}

__STL_END_NAMESPACE

#endif