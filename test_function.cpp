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
#include "stl_deque.h"
#include "stl_queue.h"
#include "stl_stack.h"

using namespace STD;

int main() {
    
    vector<int> vec;
    for (int i = 1; i < 7; ++i) {
        vec.push_back(i);
    }
    vector<int>::iterator it = find(vec.begin(), vec.end(), 5);
    if (it != vec.end()) {
        std::cout << "found" << std::endl;
    } else {
        std::cout << "not found" << std::endl;
    }
    system("pause");
    return 0;
}