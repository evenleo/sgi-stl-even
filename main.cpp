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

}

int main()
{
    vector_test();
    system("pause");
    return 0;
}