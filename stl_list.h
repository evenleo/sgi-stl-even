#ifndef __SGI_STL_INTERNAL_LIST_H
#define __SGI_STL_INTERNAL_LIST_H

__STL_BEGIN_NAMESPACE

template<class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer next;
    void_pointer prev;
    T data;
};

template<class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef __list_iterator<T, Ref, Ptr>           self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;

    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
    reference operator*() const { return (*node).data; }
    pointer operator->() const { return &(operator*()); }
    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template<class T, class Ref, class Ptr>
inline bidirectional_iterator_tag
iterator_category(const __list_iterator<T, Ref, Ptr>&) {
    return bidirectional_iterator_tag();
}

template<class T, class Ref, class Ptr>
inline T*
value_type(const __list_iterator<T, Ref, Ptr>&) {
    return 0;
}

template<class T, class Ref, class Ptr>
inline ptrdiff_t*
distance_type(const __list_iterator<T, Ref, Ptr>&) {
    return 0;
}

template <class T, class Alloc = alloc>
class list {
protected:
    typedef void* void_pointer;
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
public:
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator> reverse_iterator;

protected:
    link_type get_node() { return list_node_allocator::allocate(); }
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    link_type create_node(const T& x) {
        link_type p = get_node();
        __STL_TRY {
            construct(&p->data, x);
        }
        __STL_UNWIND(put_node(p));
        return p;
    }

    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }

protected:
    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }
    
    void fill_initialize(size_type n, const T& value) {
        empty_initialize();
        __STL_TRY {
            insert(begin(), n, value);
        }
        __STL_UNWIND(clear(); put_node(node));
    }

    template<class InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        empty_initialize();
        __STL_TRY {
            insert(begin(), first, last);
        }
        __STL_UNWIND(clear(); put_node(node));
    }

    void range_initialize(const T& first, const T& last) {
        empty_initialize();
        __STL_TRY {
            insert(begin(), first, last);
        }
        __STL_UNWIND(clear(); put_node(node));
    }

protected:
    link_type node;

public:
    list() { empty_initialize(); }
    iterator begin() { return (link_type)((*node).next); }
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return node; }
    const_iterator end() const { return node; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
     const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    bool empty() const { return node->next = node; }
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }
    size_type max_size() const { return size_type(-1); }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    void swap(list<T, Alloc>& x) { std::swap(node, x.node); }
    iterator insert(iterator position, const T& x) {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        return tmp;
    }
    iterator insert(iterator position) { return insert(position, T()); }
    template<class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
    void insert(iterator pos, size_type n, const T& x);
    void insert(iterator pos, int n, const T& x) {
        insert(pos, (size_type)n, x);
    }    
     void insert(iterator pos, long n, const T& x) {
        insert(pos, (size_type)n, x);
    }  

    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear();

    void pop_front() { erase(begin()); }
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }
    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }
    explicit list(size_type n) { fill_initialize(n, T()); }
    
    template<class InputIterator>
    list(InputIterator first, InputIterator last) {
        range_initialize(first, last);
    }
    list(const list<T, Alloc>& x) {
        range_initialize(x.begin(), x.end());
    }
    ~list() {
        clear();
        put_node(node);
    }
    list<T, Alloc>& operator=(const list<T, Alloc>& x);

protected:
    // first position last ==> last first position
    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {
            (*(link_type((*last.node).prev))).next = position.node;
            (*(link_type((*first.node).prev))).next = last.node;
            (*(link_type((*position.node).prev))).next = first.node;  
            link_type tmp = link_type((*position.node).prev);
            (*position.node).prev = (*last.node).prev;
            (*last.node).prev = (*first.node).prev; 
            (*first.node).prev = tmp;
        }
    }

public: 
    void splice(iterator position, list& x) {
        if (!x.empty())
            transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&, iterator first, iterator last) {
        if (first != last)
            transfer(position, first, last);
    }
    void remove(const T& value);
    void unique();
    void merge(list& x);
    reverse();
    sort();

    friend bool operator==(const list& x, const list& y);
};

template<class T, class Alloc>
inline bool operator==(const list<T, Alloc>& x, const list<T, Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<class T, class Alloc> template<class InputIterator>
void list<T, Alloc>::insert(iterator position,
                            InputIterator first, InputIterator last) {
    for ( ; first != last; ++first)
        insert(position, *first);
}

template<class T, class Alloc>
list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last) erase(first++);
    return last;
}

template<class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x)
{
    iterator i = begin();
    size_type len = 0;
    for ( ; i != end(); ++i, ++len)
        ;
    if (len == new_size)
        erase(i, end());
    else
        insert(end(), new_size - len, x);
}

template<class T, class Alloc>
void list<T, Alloc>::clear()
{
    link_type cur = (link_type)node->next;
    while (cur != node) {
        link_type tmp = cur;
        cur = (link_type)cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

template<class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& x) {
    if (this != &x) {
        iterator first1 = begin();
        iterator last1 = end();
        const_iterator first2 = x.begin();
        const_iterator last2 = x.end();
        while (first1 != last1 && first2 != last2) *first1++ = *first2++;
        if (first2 == last2)
            erase(first1, last1);
        else 
            insert(first1, first2, last2);
    }
    return *this;
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
        if (*first == *next)
            erase(next);
        else
            first = next;
        next = first;
    }
}




template<class T, class Alloc>
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
    x.swap(y);
}

__STL_END_NAMESPACE


#endif