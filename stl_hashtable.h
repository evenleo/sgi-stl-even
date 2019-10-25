#ifndef __SGI_STL_INTERNAL_HASHTABLE_H
#define __SGI_STL_INTERNAL_HASHTABLE_H

__STL_BEGIN_NAMESPACE

template <class Value>
struct __hashtable_node
{
    __hashtable_node* next;
    Value val;
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> 
            hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
            iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
            const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;       //迭代器目前所指之节点
    hashtable* ht;   //保持对容器的连结关系(因为需要从bucket调到bucket)

    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}  //迭代器构造函数，源码都调用这个
    __hashtable_iterator() {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++();
    iterator operator++(int);
    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> 
            hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
           iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
           const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;
    hashtable* ht;

    __hashtable_const_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
    __hashtable_const_iterator() {}
    __hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++();
    const_iterator operator++(int);
    bool operator==(const const_iterator& it) const { return cur == it.cur; }
    bool operator!=(const const_iterator& it) const { return cur != it.cur; }
};

static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53,         97,           193,         389,       769,
  1543,       3079,         6151,        12289,     24593,
  49157,      98317,        196613,      393241,    786433,
  1572869,    3145739,      6291469,     12582917,  25165843,
  50331653,   100663319,    201326611,   402653189, 805306457, 
  1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long __stl_next_prime(unsigned long n)    //找出上述28个质数之中，最接近并大于n的那个质数，如找不到则返回数值中最大质数
{
  const unsigned long* first = __stl_prime_list;
  const unsigned long* last = __stl_prime_list + __stl_num_primes;
  const unsigned long* pos = lower_bound(first, last, n); //返回[first, last) 中首个不小于（即大于或等于）n 的质数指针
  return pos == last ? *(last - 1) : *pos;
}

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey,
          class Alloc>
class hashtable {
public:
  typedef Key key_type;
  typedef Value value_type;
  typedef HashFcn hasher;
  typedef EqualKey key_equal;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  hasher hash_funct() const { return hash; }
  key_equal key_eq() const { return equals; }

private:
  hasher hash;
  key_equal equals;
  ExtractKey get_key;

  typedef __hashtable_node<Value> node;
  typedef simple_alloc<node, Alloc> node_allocator;
  vector<node*, Alloc> buckets;   //以vector完成，可动态扩容
  size_type num_elements;

public:
  typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey,
                               Alloc> iterator;
                                  
  typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey,
                                     Alloc> const_iterator;
  
  friend struct
  __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
  friend struct
  __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

public:
  hashtable(size_type n,
            const HashFcn& hf,
            const EqualKey& eql,
            const ExtractKey& ext)
    : hash(hf), equals(eql), get_key(ext), num_elements(0)
  {
    initialize_buckets(n);
  }
  hashtable(size_type n,
            const HashFcn& hf,
            const EqualKey& eql)
    : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
  {
    initialize_buckets(n);
  }
  hashtable(const hashtable& ht)
    : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), num_elements(0) 
  {
    copy_from(ht);
  }

  hashtable& operator=(const hashtable& ht) {
    if (&ht != this) {
      clear();
      hash = ht.hash;
      equals = ht.equals;
      get_key = ht.get_key;
      copy_from(ht);
    }
    return *this;
  }
  ~hashtable() { clear(); }

  size_type size() const { return num_elements; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return size() == 0; }
  
  void swap(hashtable& ht) {
    __STD::swap(hash, ht.hash);
    __STD::swap(equals, ht.equals);
    __STD::swap(get_key, ht.get_key);
    buckets.swap(ht.buckets);
    __STD::swap(num_elements, ht.num_elements);
  }
  
  iterator begin() {
    for (size_type n = 0; n < buckets.size(); ++n) 
      if (buckets[n])
        return iterator(buckets[n], this);
    return end();
  }
  iterator end() { return iterator(0, this); }
  const_iterator begin() const {
    for (size_type n = 0; n < buckets.size(); ++n) 
      if (buckets[n])
        return const_iterator(buckets[n], this);
    return end();
  }
  const_iterator end() const { return const_iterator(0, this); }

friend bool
  operator==__STL_NULL_TMPL_ARGS(const hashtable&, const hashtable&);

public:

  size_type bucket_count() const { return buckets.size(); }   //获取当前桶个数

  size_type max_bucket_count() const  //获取最大桶个数，即可以有多少个buckets
    { return __stl_prime_list[__stl_num_primes - 1]; } 

  size_type elems_in_bucket(size_type bucket) const  //获取桶元素个数
  {
    size_type result = 0;
    for (node* cur = buckets[bucket]; cur; cur = cur->next)
      result += 1;
    return result;
  }

  //插入元素，不允许重复
  pair<iterator, bool> insert_unique(const value_type& obj)
  {
    resize(num_elements + 1);   //判断是否需要重建表格，如需要，就动手...
    return insert_unique_noresize(obj);
  }

  //插入元素，允许重复
  iterator insert_equal(const value_type& obj)
  {
    resize(num_elements + 1);  //判断是否需要重建表格，如需要，就动手...
    return insert_equal_noresize(obj);
  }

  pair<iterator, bool> insert_unique_noresize(const value_type& obj);
  iterator insert_equal_noresize(const value_type& obj);
 
  template <class InputIterator>
  void insert_unique(InputIterator f, InputIterator l)
  {
    insert_unique(f, l, iterator_category(f));
  }

  template <class InputIterator>
  void insert_equal(InputIterator f, InputIterator l)
  {
    insert_equal(f, l, iterator_category(f));
  }

  template <class InputIterator>
  void insert_unique(InputIterator f, InputIterator l,
                     input_iterator_tag)
  {
    for ( ; f != l; ++f)
      insert_unique(*f);
  }

  template <class InputIterator>
  void insert_equal(InputIterator f, InputIterator l,
                    input_iterator_tag)
  {
    for ( ; f != l; ++f)
      insert_equal(*f);
  }

  template <class ForwardIterator>
  void insert_unique(ForwardIterator f, ForwardIterator l,
                     forward_iterator_tag)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_unique_noresize(*f);
  }

  template <class ForwardIterator>
  void insert_equal(ForwardIterator f, ForwardIterator l,
                    forward_iterator_tag)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_equal_noresize(*f);
  }

  reference find_or_insert(const value_type& obj);

  iterator find(const key_type& key) 
  {
    size_type n = bkt_num_key(key);
    node* first;
    for ( first = buckets[n];
          first && !equals(get_key(first->val), key);
          first = first->next)
      {}
    return iterator(first, this);
  } 

  const_iterator find(const key_type& key) const
  {
    size_type n = bkt_num_key(key);
    const node* first;
    for ( first = buckets[n];
          first && !equals(get_key(first->val), key);
          first = first->next)
      {}
    return const_iterator(first, this);
  } 

  size_type count(const key_type& key) const
  {
    const size_type n = bkt_num_key(key);
    size_type result = 0;

    for (const node* cur = buckets[n]; cur; cur = cur->next)
      if (equals(get_key(cur->val), key))
        ++result;
    return result;
  }

  pair<iterator, iterator> equal_range(const key_type& key);
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

  size_type erase(const key_type& key);
  void erase(const iterator& it);
  void erase(iterator first, iterator last);

  void erase(const const_iterator& it);
  void erase(const_iterator first, const_iterator last);

  void resize(size_type num_elements_hint);
  void clear();

private:
  //返回最接近n并大于或等于n的质素
  size_type next_size(size_type n) const { return __stl_next_prime(n); }

  void initialize_buckets(size_type n)
  {  //举例：传入50，返回53。保存53个元素空间，然后将其全部填0
    const size_type n_buckets = next_size(n);
    buckets.reserve(n_buckets);
    buckets.insert(buckets.end(), n_buckets, (node*) 0);
    num_elements = 0;
  }

  size_type bkt_num_key(const key_type& key) const  //根据key值确定落脚bucket的索引
  {
    return bkt_num_key(key, buckets.size());
  }

  size_type bkt_num(const value_type& obj) const  //根据实值确定落脚bucket的索引
  {
    return bkt_num_key(get_key(obj));
  }

  size_type bkt_num_key(const key_type& key, size_t n) const
  {
    return hash(key) % n;
  }

  size_type bkt_num(const value_type& obj, size_t n) const
  {
    return bkt_num_key(get_key(obj), n);
  }

  node* new_node(const value_type& obj)
  {
    node* n = node_allocator::allocate();
    n->next = 0;
    __STL_TRY {
      construct(&n->val, obj);
      return n;
    }
    __STL_UNWIND(node_allocator::deallocate(n));
  }
  
  void delete_node(node* n)
  {
    destroy(&n->val);
    node_allocator::deallocate(n);
  }

  void erase_bucket(const size_type n, node* first, node* last);
  void erase_bucket(const size_type n, node* last);

  void copy_from(const hashtable& ht);

};

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++()
{
  const node* old = cur;
  cur = cur->next;       //如果存在就是它，否则进入以下if流程
  if (!cur) {
    //根据元素值，定位bucket
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size()) //++bucket找到cur存在为止
      cur = ht->buckets[bucket];
  }
  return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
inline __hashtable_iterator<V, K, HF, ExK, EqK, A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
  iterator tmp = *this;
  ++*this;   //调用operator++()
  return tmp;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_const_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++()  //和非const处理流程一样
{
  const node* old = cur;
  cur = cur->next;
  if (!cur) {
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size())
      cur = ht->buckets[bucket];
  }
  return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
inline __hashtable_const_iterator<V, K, HF, ExK, EqK, A>
__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
  const_iterator tmp = *this;
  ++*this;
  return tmp;
}

template <class V, class K, class HF, class Ex, class Eq, class A>
bool operator==(const hashtable<V, K, HF, Ex, Eq, A>& ht1,
                const hashtable<V, K, HF, Ex, Eq, A>& ht2)
{
  typedef typename hashtable<V, K, HF, Ex, Eq, A>::node node;
  if (ht1.buckets.size() != ht2.buckets.size())      //buckets的size()不相等直接返回false
    return false;
  for (int n = 0; n < ht1.buckets.size(); ++n) {     //循环buckets
    node* cur1 = ht1.buckets[n];
    node* cur2 = ht2.buckets[n];
    for ( ; cur1 && cur2 && cur1->val == cur2->val;  //循环链表，循环条件两个节点存在且相等
          cur1 = cur1->next, cur2 = cur2->next)      //如果两链表相等，最后应该cur1和cur2均为0
      {}
    if (cur1 || cur2)  //只要有一个不为空则直接返回false
      return false;
  }
  return true;  //以上均不返回，则认定为两hashtable相等，返回true
}  

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class Val, class Key, class HF, class Extract, class EqKey, class A>
inline void swap(hashtable<Val, Key, HF, Extract, EqKey, A>& ht1,
                 hashtable<Val, Key, HF, Extract, EqKey, A>& ht2) {
  ht1.swap(ht2);
}

#endif 

//在不需要重建表格的情况下插入新节点。键值不允许重复
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool> 
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj)
{
  const size_type n = bkt_num(obj);  //决定obj应位于#n bucket
  node* first = buckets[n];  //令first指向bucket对应之串行头部

  //如果bucket已被占用，即first不为0，进入下面循环
  for (node* cur = first; cur; cur = cur->next) 
    if (equals(get_key(cur->val), get_key(obj)))  //如发现与链表中某键值相同，就不插入，立刻返回
      return pair<iterator, bool>(iterator(cur, this), false);

  //离开以上循环或根本不进入循环，创建新节点，插入到链表头部
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return pair<iterator, bool>(iterator(tmp, this), true);
}

//在不需要重建表格的情况下插入新节点。键值允许重复
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::iterator 
hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type& obj)
{
  const size_type n = bkt_num(obj);  //决定obj应位于#n bucket
  node* first = buckets[n];  //令first指向bucket对应之串行头部

  //如果bucket已被占用，即first不为0，进入下面循环
  for (node* cur = first; cur; cur = cur->next) 
    if (equals(get_key(cur->val), get_key(obj))) {  //如果发现与链表中某键值相同，马上插入然后返回
      node* tmp = new_node(obj);
      tmp->next = cur->next;
      cur->next = tmp;
      ++num_elements;
      return iterator(tmp, this);
    }

  //离开以上循环或根本不进入循环，创建新节点，插入到链表头部
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return iterator(tmp, this);
}

//根绝value值查找节点，找到则直接返回节点值，否则插入然后返回节点值
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::reference 
hashtable<V, K, HF, Ex, Eq, A>::find_or_insert(const value_type& obj)
{
  resize(num_elements + 1);

  size_type n = bkt_num(obj);
  node* first = buckets[n];

  for (node* cur = first; cur; cur = cur->next)
    if (equals(get_key(cur->val), get_key(obj)))  //找到则直接返回节点值
      return cur->val;
  
  //否则插入然后返回节点值
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return tmp->val;
}

//获取键值为key的左开右闭区间
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator,
     typename hashtable<V, K, HF, Ex, Eq, A>::iterator> 
hashtable<V, K, HF, Ex, Eq, A>::equal_range(const key_type& key)
{
  typedef pair<iterator, iterator> pii;
  const size_type n = bkt_num_key(key);  //确定buckets的位置

  for (node* first = buckets[n]; first; first = first->next) {
    if (equals(get_key(first->val), key)) {  //循环链表，找到第一个键值与key相等的节点first
      for (node* cur = first->next; cur; cur = cur->next)
        if (!equals(get_key(cur->val), key))  //如果在当前bucket后面找到第一个键值与key不相等的节点cur
          return pii(iterator(first, this), iterator(cur, this));  //[first, cur)就是所求
      for (size_type m = n + 1; m < buckets.size(); ++m)  //如果在后面buckets继续找不为空的bucket
        if (buckets[m])                                   //[first, buckets[m])就是所求
          return pii(iterator(first, this),
                     iterator(buckets[m], this));
      return pii(iterator(first, this), end());  //上面不返回，这里[first, end())就是所求
    }
  }
  return pii(end(), end());  //没找到，返回[end(), end())
}

//获取键值为key的左开右闭区间，分析过程和上面一样
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator, 
     typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator> 
hashtable<V, K, HF, Ex, Eq, A>::equal_range(const key_type& key) const
{
  typedef pair<const_iterator, const_iterator> pii;
  const size_type n = bkt_num_key(key);

  for (const node* first = buckets[n] ; first; first = first->next) {
    if (equals(get_key(first->val), key)) {
      for (const node* cur = first->next; cur; cur = cur->next)
        if (!equals(get_key(cur->val), key))
          return pii(const_iterator(first, this),
                     const_iterator(cur, this));
      for (size_type m = n + 1; m < buckets.size(); ++m)
        if (buckets[m])
          return pii(const_iterator(first, this),
                     const_iterator(buckets[m], this));
      return pii(const_iterator(first, this), end());
    }
  }
  return pii(end(), end());
}

template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::size_type 
hashtable<V, K, HF, Ex, Eq, A>::erase(const key_type& key)
{
  const size_type n = bkt_num_key(key);  //获取buckets的位置
  node* first = buckets[n];
  size_type erased = 0;

  if (first) {
    node* cur = first;
    node* next = cur->next;
    while (next) {  //删除头结点后面所有键值和key相等的节点
      if (equals(get_key(next->val), key)) {
        cur->next = next->next;
        delete_node(next);
        next = cur->next;
        ++erased;
        --num_elements;
      }
      else {
        cur = next;
        next = cur->next;
      }
    }
    if (equals(get_key(first->val), key)) {  //如果头结点键值也等于key，删除头结点
      buckets[n] = first->next;
      delete_node(first);
      ++erased;
      --num_elements;
    }
  }
  return erased;  //返回删除的元素个数
}

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::erase(const iterator& it)
{
  if (node* const p = it.cur) {  //如果it.cur不为空则进入以下删除操作
    const size_type n = bkt_num(p->val);  //定位p的buckets位置
    node* cur = buckets[n];

    if (cur == p) {  //p为头结点，直接执行删除
      buckets[n] = cur->next;
      delete_node(cur);
      --num_elements;
    }
    else {  //下面循环找到p的位置删除
      node* next = cur->next;
      while (next) {
        if (next == p) {
          cur->next = next->next;
          delete_node(next);
          --num_elements;
          break;
        }
        else {
          cur = next;
          next = cur->next;
        }
      }
    }
  }
}

//删除指定区间[first, last)的元素
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::erase(iterator first, iterator last)
{
  size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
  size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();

  if (first.cur == last.cur)
    return;
  else if (f_bucket == l_bucket)
    erase_bucket(f_bucket, first.cur, last.cur);
  else {
    erase_bucket(f_bucket, first.cur, 0);
    for (size_type n = f_bucket + 1; n < l_bucket; ++n)
      erase_bucket(n, 0);
    if (l_bucket != buckets.size())
      erase_bucket(l_bucket, last.cur);
  }
}

//同上，删除指定区间[first, last)的元素
template <class V, class K, class HF, class Ex, class Eq, class A>
inline void
hashtable<V, K, HF, Ex, Eq, A>::erase(const_iterator first,
                                      const_iterator last)
{
  erase(iterator(const_cast<node*>(first.cur),
                 const_cast<hashtable*>(first.ht)),
        iterator(const_cast<node*>(last.cur),
                 const_cast<hashtable*>(last.ht)));
}

template <class V, class K, class HF, class Ex, class Eq, class A>
inline void
hashtable<V, K, HF, Ex, Eq, A>::erase(const const_iterator& it)
{
  erase(iterator(const_cast<node*>(it.cur),
                 const_cast<hashtable*>(it.ht)));
}

//以下函数判断是否需要重建表格。如果不需要立刻返回。如果需要，就动手...
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint)
{
  const size_type old_n = buckets.size();
  if (num_elements_hint > old_n) {  //元素个数大于vector的大小，就重建表格
    const size_type n = next_size(num_elements_hint);  //找下一个质数
    if (n > old_n) {
      vector<node*, A> tmp(n, (node*) 0);  //建立新的buckets
      __STL_TRY {
        for (size_type bucket = 0; bucket < old_n; ++bucket) {  //循环处理每一个旧bucket
          node* first = buckets[bucket];
          while (first) {  //循环将链表的值复制到新的bucket，过程链表操作不难理解
            size_type new_bucket = bkt_num(first->val, n);
            buckets[bucket] = first->next;
            first->next = tmp[new_bucket];
            tmp[new_bucket] = first;
            first = buckets[bucket];          
          }
        }
        buckets.swap(tmp);  //两个buckets对调，tmp是局部变量，离开时释放内存
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {  //抛出异常释放tmp内存空间
        for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
          while (tmp[bucket]) {
            node* next = tmp[bucket]->next;
            delete_node(tmp[bucket]);
            tmp[bucket] = next;
          }
        }
        throw;
      }
#         endif
    }
  }
}

//删除指定映射的所有元素
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::erase_bucket(const size_type n, 
                                                  node* first, node* last)
{
  node* cur = buckets[n];
  if (cur == first)  //如果first为bucket的头结点，调用下面erase_bucket的重载函数
    erase_bucket(n, last);
  else {
    node* next;
    //找出链表中first的位置next
    for (next = cur->next; next != first; cur = next, next = cur->next)
      ;
    while (next) {  //然后删除至链表结尾, 这里有个疑问：条件不应该是next！= last吗？
      cur->next = next->next;
      delete_node(next);
      next = cur->next;
      --num_elements;
    }
  }
}

//删除buckets[n]中到last之前的节点
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::erase_bucket(const size_type n, node* last)
{
  node* cur = buckets[n];  
  while (cur != last) {
    node* next = cur->next;
    delete_node(cur);
    cur = next;
    buckets[n] = cur;
    --num_elements;
  }
}

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::clear()
{
  for (size_type i = 0; i < buckets.size(); ++i) {  //循环buckets
    node* cur = buckets[i];  //逐个删除链表节点
    while (cur != 0) {
      node* next = cur->next;
      delete_node(cur);
      cur = next;
    }
    buckets[i] = 0;  //将桶置为空
  }
  num_elements = 0;  //元素数量置为0
}

    
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht)
{
  buckets.clear();  //清空buckets。vector操作
  buckets.reserve(ht.buckets.size());  //buckets大小置为ht的一样大小
  buckets.insert(buckets.end(), ht.buckets.size(), (node*) 0);  //将每个bucket置为0
  __STL_TRY {
    for (size_type i = 0; i < ht.buckets.size(); ++i) {  //循环buckets
      if (const node* cur = ht.buckets[i]) {  //ht.buckets[i]该桶不为空，进入以下复制操作
        node* copy = new_node(cur->val);  //将链表第一个节点复制到buckets[i]
        buckets[i] = copy;
        //将链表后续的节点循环复制
        for (node* next = cur->next; next; cur = next, next = cur->next) {
          copy->next = new_node(next->val);
          copy = copy->next;
        }
      }
    }
    num_elements = ht.num_elements;  //元素个数置为ht的一样
  }
  __STL_UNWIND(clear());
}

__STL_END_NAMESPACE

#endif