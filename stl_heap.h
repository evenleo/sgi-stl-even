#ifndef __SGI_STL_INTERNAL_HEAP_H
#define __SGI_STL_INTERNAL_HEAP_H

__STL_BEGIN_NAMESPACE

template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator first, Distance holeIndex,
                 Distance topIndex, T value) {
  Distance parent = (holeIndex - 1) / 2;  //找出父节点
  while (holeIndex > topIndex && *(first + parent) < value) {
    //尚未到达顶端且父节点小于洞值，使用operator<，知max-heap
    *(first + holeIndex) = *(first + parent);  //令洞值为父值
    holeIndex = parent;                        //新洞为父节点
    parent = (holeIndex - 1) / 2;              //新洞的父节点
  }    
  *(first + holeIndex) = value;  //令洞值为新值
}

template <class RandomAccessIterator, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first,
                            RandomAccessIterator last, Distance*, T*) {
  __push_heap(first, Distance((last - first) - 1), Distance(0), 
              T(*(last - 1)));
  //(last-first)–1代表新元素的索引，0是堆首的索引，*(last - 1)是新加入的值              
}

template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
  //注意，调用该函数时候，新元素位于最后一个位置(last-1)
  __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
                   Distance len, T value) {
  Distance topIndex = holeIndex;
  Distance secondChild = 2 * holeIndex + 2;  //洞节点的右子节点
  while (secondChild < len) {
    if (*(first + secondChild) < *(first + (secondChild - 1)))  //左节点值大于右节点值
      secondChild--;  //洞节点往左下放
    *(first + holeIndex) = *(first + secondChild);  //洞节点下放
    holeIndex = secondChild;  //新洞节点
    secondChild = 2 * (secondChild + 1);  //新洞节点的右子节点
  }
  if (secondChild == len) {  //如果只有左子节点，洞节点下放到左子节点
    *(first + holeIndex) = *(first + (secondChild - 1));
    holeIndex = secondChild - 1;
  }
  __push_heap(first, holeIndex, topIndex, value);  //原尾值插入到新数组中，上溯
}

template <class RandomAccessIterator, class T, class Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator result, T value, Distance*) {
  *result = *first;  //设定尾值为首值，于是尾值即是结果，
	//可由调用底层容器之 pop_back() 取出尾值
  __adjust_heap(first, Distance(0), Distance(last - first), value);
  //以上欲重新调整 heap，洞号为 0，欲调整值为value
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first,
                           RandomAccessIterator last, T*) {
  __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
  //pop动作的结果为底层容器的第一个元素。因此，首先设定欲调整值为尾值，然后將首值调至 
	//尾节点（所以以上将迭代器result设为last-1）。然后重整 [first, last-1)，
	//使之重新成一个合格的 heap
}

template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
  __pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*,
                 Distance*) {
  if (last - first < 2) return;  //如果长度小于2，不必排序直接退出
  Distance len = last - first;   
  //找出第一个需要重排的子树头部，以 parent 标示出。由于任何叶节点都不需执行 
	//perlocate down，所以有以下計算
  Distance parent = (len - 2)/2;
    
  while (true) {
    //调整以 parent 为首的子树。len 是为了 __adjust_heap() 判断做范围
    __adjust_heap(first, parent, len, T(*(first + parent)));
    if (parent == 0) return;  //走完根节点结束
    parent--;  //重排子树头部退一格
  }
}

template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
  __make_heap(first, last, value_type(first), distance_type(first));
}

//每执行一次 pop_heap()，极值（在STL heap中为极大值）即被放在尾端。
//扣除尾端再执行一次 pop_heap()，次极值又被放在新尾端。一直下去，最后即得排序結果。
//每执行 pop_heap() 一次，操作范围即退缩一格
template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
  while (last - first > 1) pop_heap(first, last--);
}

/************以下允许指定大小比较标准，原理一样，不再注释****************/

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __push_heap(RandomAccessIterator first, Distance holeIndex,
                 Distance topIndex, T value, Compare comp) {
  Distance parent = (holeIndex - 1) / 2;
  while (holeIndex > topIndex && comp(*(first + parent), value)) {
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

template <class RandomAccessIterator, class Compare, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first,
                            RandomAccessIterator last, Compare comp,
                            Distance*, T*) {
  __push_heap(first, Distance((last - first) - 1), Distance(0), 
              T(*(last - 1)), comp);
}

template <class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last,
                      Compare comp) {
  __push_heap_aux(first, last, comp, distance_type(first), value_type(first));
}

template <class RandomAccessIterator, class Distance, class T, class Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
                   Distance len, T value, Compare comp) {
  Distance topIndex = holeIndex;
  Distance secondChild = 2 * holeIndex + 2;
  while (secondChild < len) {
    if (comp(*(first + secondChild), *(first + (secondChild - 1))))
      secondChild--;
    *(first + holeIndex) = *(first + secondChild);
    holeIndex = secondChild;
    secondChild = 2 * (secondChild + 1);
  }
  if (secondChild == len) {
    *(first + holeIndex) = *(first + (secondChild - 1));
    holeIndex = secondChild - 1;
  }
  __push_heap(first, holeIndex, topIndex, value, comp);
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator result, T value, Compare comp,
                       Distance*) {
  *result = *first;
  __adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template <class RandomAccessIterator, class T, class Compare>
inline void __pop_heap_aux(RandomAccessIterator first,
                           RandomAccessIterator last, T*, Compare comp) {
  __pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp,
             distance_type(first));
}

template <class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                     Compare comp) {
    __pop_heap_aux(first, last, value_type(first), comp);
}

template <class RandomAccessIterator, class Compare, class T, class Distance>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last,
                 Compare comp, T*, Distance*) {
  if (last - first < 2) return;
  Distance len = last - first;
  Distance parent = (len - 2)/2;
    
  while (true) {
    __adjust_heap(first, parent, len, T(*(first + parent)), comp);
    if (parent == 0) return;
    parent--;
  }
}

template <class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last,
                      Compare comp) {
  __make_heap(first, last, comp, value_type(first), distance_type(first));
}

template <class RandomAccessIterator, class Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last,
               Compare comp) {
  while (last - first > 1) pop_heap(first, last--, comp);
}

__STL_END_NAMESPACE

#endif