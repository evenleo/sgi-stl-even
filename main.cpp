#include <iostream>

#include "stl_config.h"
#include "type_traits.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_construct.h"
#include "stl_relops.h"
#include "stl_pair.h"
#include "stl_algobase.h"
#include "stl_uninitialized.h"
#include "stl_vector.h"
#include "stl_list.h" 
#include "stl_bvector.h"
#include "stl_function.h"
#include "stl_tree.h"

// #include <vecotor_sgi>

void list_test() {
    STD::list<int> list;
    for (int i=0; i<10; ++i) {
        list.push_back(i);
    }
    
    STD::list<int>::iterator iter = list.begin();
    for ( ; iter != list.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

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

void bool_ptr_test() {
    bool arr[10];
    bool *p = arr;
    bool *p2 = p++;

}

int main()
{
    STD::less<int> key_compare;
    bool r1 = key_compare(2, 3);
    bool r2 = key_compare(3, 3);
    bool r3 = key_compare(4, 3);


    // stl_tree_test();

    system("pause");
    return 0;
}