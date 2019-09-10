#include <iostream>

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_list.h"
#include "skiplist.h"

using namespace std;


int main()
{
    skiplist<int> ii;
    for(int i=1; i< 50; i++){
        if((i%3) == 0){
            ii.insert(i);
        }
    }
    for(int i=1; i< 50; i++){
        if((i%3) == 1){
            ii.insert(i);
        }
    }
    ii.printAll();
    ii.printAll(-1);

    system("pause");
    return 0;
}