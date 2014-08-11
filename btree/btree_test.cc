#include "btree_test.h"

#include <iostream>
#include <sstream>
#include <random>
#include <map>
#include <Windows.h>
#include <DbgHelp.h>

typedef algo::btree<int,int,3> tree_t;

bool assert_tree(tree_t& tr, const std::string& expected)
{
    std::stringstream ss;
    for(auto it = tr.begin(); it != tr.end(); ++it)
    {
        ss << it->first << ',';
    }
    return ss.str() == expected;
}

static size_t gErrors = 0;
#define TESTCASE_EVAL(expr)\
{ bool r = expr; if(!r) ++gErrors; std::cout << "Case " << #expr << ": "  << (r ? "PASSED" : "FAILED") << "\n"; }

void run_test_cases()
{
    tree_t tr;
    tr.insert(std::make_pair(1, 0));
    tr.insert(std::make_pair(2, 0));
    tr.insert(std::make_pair(3, 0));
    tr.insert(std::make_pair(4, 0));
    tr.insert(std::make_pair(5, 0));
    tr.insert(std::make_pair(6, 0));
    tr.insert(std::make_pair(7, 0));
    tr.insert(std::make_pair(8, 0));
    tr.insert(std::make_pair(9, 0));

    tr.erase(4);
    TESTCASE_EVAL(assert_tree(tr, "1,2,3,5,6,7,8,9,"));

    tr.insert(std::make_pair(20, 0));
    tr.insert(std::make_pair(21, 0));
    tr.insert(std::make_pair(22, 0));
    tr.insert(std::make_pair(10, 0));
    tr.insert(std::make_pair(11, 0));
    tr.insert(std::make_pair(12, 0));
    TESTCASE_EVAL(assert_tree(tr, "1,2,3,5,6,7,8,9,10,11,12,20,21,22,"));

    tr.erase(10);
    TESTCASE_EVAL(assert_tree(tr, "1,2,3,5,6,7,8,9,11,12,20,21,22,"));
    tr.insert(std::make_pair(10, 1));
    TESTCASE_EVAL(assert_tree(tr, "1,2,3,5,6,7,8,9,10,11,12,20,21,22,"));
    tr.erase(12);
    TESTCASE_EVAL(assert_tree(tr, "1,2,3,5,6,7,8,9,10,11,20,21,22,"));
    tr.erase(1);
    TESTCASE_EVAL(assert_tree(tr, "2,3,5,6,7,8,9,10,11,20,21,22,"));
    tr.erase(22);
    TESTCASE_EVAL(assert_tree(tr, "2,3,5,6,7,8,9,10,11,20,21,"));
    tr.erase(9);
    TESTCASE_EVAL(assert_tree(tr, "2,3,5,6,7,8,10,11,20,21,"));

    TESTCASE_EVAL(tr.find(5)->first == 5);
    TESTCASE_EVAL(tr.find(21)->first == 21);
    TESTCASE_EVAL(tr.find(10)->first == 10);
    TESTCASE_EVAL(tr.find(100) == tr.end());
    TESTCASE_EVAL(tr.find(1) == tr.end());
}

#define PERFORMANCE_EVAL(expr)\
{::QueryPerformanceCounter(pStart);\
{ expr; }\
    ::QueryPerformanceCounter(pEnd);\
    std::cout << #expr << " : " << 1000000 * (liEnd - liStart) / liFreq << " micro seconds.\n"; \
}

template<typename Map, typename Vec>
void performance_test_insert(Map& m, const Vec& v, size_t n)
{
    for(size_t i = 0; i < n; ++i)
    {
        m.insert(v[i]);
    }
}

template<typename Map, typename Vec>
void performance_test_erase(Map& m, const Vec& v, size_t n)
{
    for(size_t i = 0; i < n; ++i)
    {
        m.erase(v[i].first);
    }
}

bool performance_test()
{
    const size_t N = 5000000;

    std::vector<std::pair<int,int> > randoms;
    randoms.reserve(N);

    for(int i = 0; i < N; ++i)
    {
        randoms.push_back(std::make_pair(i, 0));
    }
    std::random_shuffle(randoms.begin(), randoms.end());


    unsigned long long liFreq;
    unsigned long long liStart, liEnd;
    LARGE_INTEGER* pFreq  = (LARGE_INTEGER*)&liFreq;
    LARGE_INTEGER* pStart = (LARGE_INTEGER*)&liStart;
    LARGE_INTEGER* pEnd   = (LARGE_INTEGER*)&liEnd;
    ::QueryPerformanceFrequency(pFreq);

    // compare performance with std::map
    std::map<int,int> std_map;
    algo::btree<int, int, 128> btree_map;
    //PERFORMANCE_EVAL(performance_test_insert(std_map, randoms, N));
    PERFORMANCE_EVAL(performance_test_insert(btree_map, randoms, N));

    std::random_shuffle(randoms.begin(), randoms.end());
    //PERFORMANCE_EVAL(performance_test_erase(std_map, randoms, N));
    PERFORMANCE_EVAL(performance_test_erase(btree_map, randoms, N));
    return true;
}

struct A{
    char d1;
    int  d2;
    char d3;
};

struct B{
    int  d1;
    double d2;
    char d3;
};

struct C{
    char d1;
    A    d2;
    int  d3;
    B    d4;
    char d5;
};

int off(void* p1, void* p2)
{
    return (char*)p2 - (char*)p1;
}

int main(int argc, char* argv[])
{
    std::cout << "sizeof(A): " << sizeof(A)
            << "\nsizeof(B): " << sizeof(B)
            << "\nsizeof(C): " << sizeof(C) << '\n';
    
    C c;
    std::cout << "offset C::d1: " << off(&c, &c.d1) << '\n';
    std::cout << "offset C::d2: " << off(&c, &c.d2) << '\n';
    std::cout << "offset C::d3: " << off(&c, &c.d3) << '\n';
    std::cout << "offset C::d4: " << off(&c, &c.d4) << '\n';
    return 0;

    run_test_cases();
    _CrtDumpMemoryLeaks();

    if( !gErrors )
        std::cout << " ALL TEST CASES ARE PASSED SUCCESSFULLY.\n";
    else
        std::cout << " !!! ERROR ERROR ERROR ERROR !!! TEST IS FAILED.\n";

    std::cout << "\n ---- Begin performance test ----\n";
    performance_test();
    std::cout << "\n ---- End performance test ----\n";

    return 0;
}
