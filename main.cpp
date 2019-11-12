#include <iostream>
#include <string>

#include "stl_config.h"
#include "type_traits.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_construct.h"
#include "stl_relops.h"
#include "stl_pair.h"
#include "stl_algobase.h"
#include "stl_algo.h"
#include "stl_uninitialized.h"
#include "stl_function.h"
#include "stl_hash_fun.h"
#include "stl_vector.h"
#include "stl_list.h" 
#include "stl_bvector.h"
#include "stl_function.h"
#include "stl_tree.h"
#include "stl_slist.h"
#include "stl_set.h"
#include "stl_multiset.h"
#include "stl_hashtable.h"
#include "stl_hash_set.h"
#include "stl_map.h"
#include "stl_multimap.h"
#include "stl_heap.h"
#include "stl_queue.h"
#include "stl_deque.h"
#include "stl_stack.h"

/**************************list test*******************************/

template<class T>
void print(const STD::list<T>& list) { //打印链表
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

template<class T>
void sort(STD::list<T>& list) {
    if (list.size() <= 1) return;
    STD::list<T> carry;
    STD::list<T> counter[64];
    int fill = 0;
    while (!list.empty()) {
        carry.splice(carry.begin(), list, list.begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
        
        //打印本次循环的所有递归层次内容
        std::cout << "---------test------------------" << std::endl;
        for (int i = 0; i < fill; ++i) {
            std::cout << "counter[" << i << "]: ";
            print(counter[i]);
        }
    }

    for (int i = 1; i < fill; ++i) counter[i].merge(counter[i-1]);
    list.swap(counter[fill-1]);
}

void list_test() {
    STD::list<int> list;
    for (int i=0; i<10; ++i) {
        int r = rand() % 10;
        list.push_back(r);
    }
    
    STD::list<int>::iterator iter = list.begin();
    for ( ; iter != list.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    std::cout << "begin sort:" << std::endl;
    sort(list);
    std::cout << "after sort:" << std::endl;
    iter = list.begin();
    for ( ; iter != list.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

/**************************list test*******************************/

 
struct MM {
    int val;
    MM(int x) : val(x) {}
};

void vector_test2() {
    STD::vector<MM> vec(2, MM(22));
    for (auto i : vec) {
        std::cout << i.val << " ";
    }
    std::cout << std::endl;
}

void vector_test() {
    STD::vector<int> vec(2, 11);
    for (int i=0; i<10; ++i) {
        vec.push_back(i);
    }
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    STD::vector<int>::iterator iter = vec.begin();
    vec.insert(iter+2, 1, 10);
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    iter = vec.begin();
    vec.erase(iter, iter+2);
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void bit_vector_test() {
    std::cout << "sizeof(bool):" << sizeof(unsigned int) << std::endl;
    std::cout << CHAR_BIT << std::endl;
    int WORD_BIT = int(CHAR_BIT*sizeof(unsigned int));
    std::cout << WORD_BIT << std::endl;
    STD::vector<bool> vec;
    vec.push_back(true);
    vec.push_back(false);
    vec.push_back(true);

    vec.push_back(false);
    vec.push_back(true);
    vec.push_back(false);
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void stl_tree_test() {
    typedef STD::rb_tree<int, int, STD::identity<int>, STD::less<int>> RB_tree;
    RB_tree itree;
    std::cout << itree.size() << std::endl;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);

    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);

    itree.insert_unique(11);

    itree.insert_unique(7);

    itree.insert_unique(13);
    itree.insert_unique(12);

    std::cout << itree.size() << std::endl;
    RB_tree::iterator it = itree.begin();
    for ( ; it != itree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    for (it = itree.begin(); it != itree.end(); ++it) {
        STD::__rb_tree_base_iterator rbtite = STD::__rb_tree_base_iterator(it);
        std::cout << *it << "(" << rbtite.node->color << ") ";
    }
    std::cout << std::endl;
}

void slist_test() {
    STD::slist<int> sl;
    sl.push_front(1);
    sl.push_front(2);
    sl.push_front(3);
    for (auto it = sl.begin(); it != sl.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    STD::slist<int>::iterator it = sl.begin();
    ++it;
    sl.insert_after(it, 4);

    for (auto it = sl.begin(); it != sl.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void set_test() {
    STD::set<int> iset;
    for (int i = 0; i < 1000; ++i) {
        int r = rand() % 100;
        iset.insert(r);
    }
    for (auto it = iset.begin(); it != iset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    int targert = 9;
    auto it = iset.find(targert);
    if (it != iset.end()) {
        std::cout << "found " << targert << std::endl;
    } else {
        std::cout << "not found " << targert << std::endl;
    }
    size_t cnt = iset.count(targert);
    std::cout << targert << " has " << cnt << std::endl;
}

void multiset_test() {
    STD::multiset<int> iset;
    for (int i = 0; i < 1000; ++i) {
        int r = rand() % 100;
        iset.insert(r);
    }
    for (auto it = iset.begin(); it != iset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    int targert = 9;
    auto it = iset.find(targert);
    if (it != iset.end()) {
        std::cout << "found " << targert << std::endl;
    } else {
        std::cout << "not found " << targert << std::endl;
    }
    size_t cnt = iset.count(targert);
    std::cout << targert << " has " << cnt << std::endl;
}

struct eqstr {
    bool operator() (const char* a, const char* b) const {
        return strcmp(a, b) == 0;
    }
};

void hashtable_test() {
    STD::hashtable<const char*, 
                   const char*, 
                   STD::hash<const char*>, 
                   STD::identity<const char*>,
                   eqstr, STD::alloc> 
    ht(50, STD::hash<const char*>(), eqstr());
    ht.insert_unique("hello");
    ht.insert_unique("even");
    ht.insert_unique("haha");
    for (auto it = ht.begin(); it != ht.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
 
    std::cout << "test copy constructor" << std::endl;
    STD::hashtable<const char*, 
                   const char*, 
                   STD::hash<const char*>, 
                   STD::identity<const char*>,
                   eqstr, STD::alloc> 
    tmp = ht;
    for (auto it = tmp.begin(); it != tmp.end(); ++ it) {
        std::cout << *it << " ";
    }        
    std::cout << std::endl;           
}

void hash_set_test() {
    STD::hash_set<int,
                  STD::hash<int>>
    hset(50);
    hset.insert(2);
    for (int i = 0; i < 53; ++i) {
        hset.insert(i);
    }
    for (auto it = hset.begin(); it != hset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << hset.bucket_count() << std::endl;
    hset.insert(2);
    std::cout << hset.bucket_count() << std::endl;
    hset.insert(100);
    for (auto it = hset.begin(); it != hset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << hset.bucket_count() << std::endl;
}

void hash_multiset_test() {
    STD::hash_multiset<int,
                  STD::hash<int>>
    hmset(50);
    hmset.insert(2);
    for (int i = 0; i < 53; ++i) {
        hmset.insert(i);
    }
    for (auto it = hmset.begin(); it != hmset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << hmset.bucket_count() << std::endl;
    hmset.insert(100);
    for (auto it = hmset.begin(); it != hmset.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << hmset.bucket_count() << std::endl;
}

void map_test() {
    STD::map<int, std::string> map;
    map.insert(STD::make_pair<int, std::string>(101, "even"));
    map.insert(STD::make_pair<int, std::string>(102, "cury"));
    map.insert(STD::make_pair<int, std::string>(103, "olvin"));
    map.insert(STD::make_pair<int, std::string>(104, "onio"));
    map.insert(STD::make_pair<int, std::string>(104, "koko"));
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "map[103]:" << map[103] << std::endl;
    std::cout << "map[105]:" << map[105] << std::endl;
}

void multimap_test() {
    STD::multimap<int, std::string> mutimap;
    mutimap.insert(STD::make_pair<int, std::string>(101, "even"));
    mutimap.insert(STD::make_pair<int, std::string>(102, "cury"));
    mutimap.insert(STD::make_pair<int, std::string>(103, "olvin"));
    mutimap.insert(STD::make_pair<int, std::string>(104, "onio"));
    mutimap.insert(STD::make_pair<int, std::string>(104, "koko"));
    for (auto it = mutimap.begin(); it != mutimap.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "test lower_bound and upper_bound" << std::endl;
    auto lower_it = mutimap.lower_bound(104);
    auto upper_it = mutimap.upper_bound(104);
    for ( ; lower_it != upper_it; ++lower_it) {
        std::cout << lower_it->first << ": " << lower_it->second << std::endl;
    }
}

void test_heap() {
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    STD::vector<int> vec(ia, ia+9);
    STD::make_heap(vec.begin(), vec.end());
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "sort_heap:" << std::endl;
    STD::sort_heap(vec.begin(), vec.end());
    for (auto i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void priority_queue_test() {
    STD::priority_queue<int> pqueue;
    for (int i = 0; i < 100; ++i) {
        int r = rand() % 100;
        pqueue.push(r);
    }

    for (int i = 0; i < 100; ++i) {
        std::cout << pqueue.top() << " ";
        pqueue.pop();
    }
    std::cout << std::endl;
}

void offset_test() {
    int size = 8;
    int cur = 4;
    int first = 0;
    while (1) {
        int n = 0;
        std::cout << "input n: ";
        std::cin >> n;
        int offset = n + (cur - first);
        if (offset >= 0 && offset < size) {
            std::cout << "cur + offset" << std::endl;
        } else {
            if (offset > 0) {
                std::cout << "offset / size: " << offset / size << std::endl;
            } else {
                std::cout << "-((-offset - 1) / size) - 1: " 
                <<-((-offset - 1) / size) - 1 << std::endl;
                 std::cout << "offset / size: " 
                 << offset / size << std::endl;
            }
        }
    }
}

void deque_test() {
    STD::deque<int> deque;
    deque.push_back(10);
    deque.push_back(11);
    deque.push_back(12);
    deque.push_back(13);
    deque.push_front(9);
    deque.push_front(8);
    for (auto i : deque) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void stack_test() {  // 后进先出
    STD::stack<int> stack;
    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }
    while (!stack.empty()) {
        std::cout << stack.top() << " ";
        stack.pop();
    }
}

int main()
{
    stack_test();

    system("pause");
    return 0;
}