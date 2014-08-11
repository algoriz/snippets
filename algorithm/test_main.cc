#include "test_cases.h"
#include <iostream>
#include "algorithm.h"

unsigned convertDigit(char c)
{
    static const unsigned digit_map[] = {
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,0,1,2,3,4,5,6,7,8,9,255,255,255,255,
        255,255,255,10,11,12,13,14,15,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,10,11,12,13,14,15,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
    };

    return digit_map[c];
}

int main(int argc, char* argv[])
{
    //std::cout << "Running test cases...\n";

    //CASE_sort_basic();
    //CASE_sort_random();
    //CASE_bsearch_basic();
    //CASE_lubound_basic();
    //CASE_slist();
    //CASE_disjoint_set();
    CASE_linear_select();

    return 0;
}
