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



int main()
{
    STD::list<int> list;
    for (int i=0; i<10; ++i) {
        list.push_back(i);
    }
    
    STD::list<int>::iterator iter = list.begin();
    for ( ; iter != list.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    system("pause");
    return 0;
}