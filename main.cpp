#include <iostream>

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_list.h"
#include "skiplist.h"

using namespace std;


int main()
{
    skiplist<int> ii{1, 2, 3, 4, 5};
    ii.pintAll();

    system("pause");
    return 0;
}