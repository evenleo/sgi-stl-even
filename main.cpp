#include <iostream>

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_list.h"
#include "skiplist.h"

using namespace std;


int main()
{
    skiplist<int> *list = new skiplist<int>;
    list->test();
    system("pause");
    return 0;
}