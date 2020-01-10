#ifndef __SGI_STL_INTERNAL_ALGO_H
#define __SGI_STL_INTERNAL_ALGO_H

__STL_BEGIN_NAMESPACE

template <class T>
inline T& __median(const T& a, const T& b, const T& c) {
    if (a < b)
        if (b < c) 
            return b;
        else if (a < c) 
            return c;
        else
            return a;
    else if (a < c)
        return a;
    else if (b < c) 
        return c;
    else 
        return b;
}

template <class T, class Compare>
inline T& __median(const T& a, const T& b, const T& c, Compare comp) {
  if (comp(a, b))
    if (comp(b, c))
      return b;
    else if (comp(a, c))
      return c;
    else
      return a;
  else if (comp(a, c))
    return a;
  else if (comp(b, c))
    return c;
  else
    return b;
}

template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
    for ( ; first != last; ++first)
        f(*first);
    return f;
}

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    while (first != last && *first != value) ++first;
    return first;
}

template <class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last,
                      Predicate pred) {
    while (first != last && !pred(*first)) ++first;
    return first;
}



template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, 
                              const T& value, Distance*,
                              forward_iterator_tag) {
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else 
            len = half;
    }
    return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Distance*,
                                   random_access_iterator_tag) {
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (*middle < value) {
            first = middle + 1;
            len = len - half - 1;
        } else 
            len = half;
    }
    return first;
}

//返回指向范围 [first, last) 中首个不小于（即大于或等于）value 的元素的迭代器，或若找不到这种元素则返回 last 
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value) {
    return __lower_bound(first, last, value, distance_type(first),
                         iterator_category(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, 
                              const T& value, Compare comp, Distance*,
                              forward_iterator_tag) {
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (comp(*middle, value)) {
            first = middle;
            ++first;
            len = len - half - 1;
        } else 
            len = half;
    }
    return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T& value, Compare comp, Distance*,
                                   random_access_iterator_tag) {
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;

    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (comp(*middle, value)) {
            first = middle + 1;
            len = len - half - 1;
        } else 
            len = half;
    }
    return first;
}

//返回指向范围 [first, last) 中首个不小于（即大于或等于）value 的元素的迭代器，或若找不到这种元素则返回 last
template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp) {
    return __lower_bound(first, last, value, comp, distance_type(first),
                         iterator_category(first));
}




__STL_END_NAMESPACE






#endif