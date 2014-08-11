#include "test_cases.h"
#include "algorithm.h"
#include "slist.h"
#include "disjoint_set.h"
#include "linear_select.h"

#include <random>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>

class reversed_less
{
public:
    template<typename T>
    bool operator() (const T& t1, const T& t2) const
    {
        return t1 > t2;
    }
};


void generate_randoms(std::vector<int>& randoms)
{
    static int seed = 17451;

    randoms.clear();
    randoms.reserve(1000);

    std::srand(++seed);
    for (int i = 0; i < 1000; ++i)
    {
        randoms.push_back(std::rand());
    }
}

size_t quick_sort_test(const std::vector<int>& input, const std::vector<int>& expected)
{
    std::vector<int> s(input);
    algo::quick_sort(s.begin(), s.end());
    return s == expected ? 0 : 1;
}

size_t quick_sort_pred_test(const std::vector<int>& input, const std::vector<int>& expected)
{
    std::vector<int> s(input);
    algo::quick_sort(s.begin(), s.end(), reversed_less());
    return s == expected ? 0 : 1;
}

size_t heap_sort_test(const std::vector<int>& input, const std::vector<int>& expected)
{
    std::vector<int> s(input);
    algo::heap_sort(s.begin(), s.end());
    return s == expected ? 0 : 1;
}

size_t heap_sort_pred_test(const std::vector<int>& input, const std::vector<int>& expected)
{
    std::vector<int> s(input);
    algo::heap_sort(s.begin(), s.end(), reversed_less());
    return s == expected ? 0 : 1;
}

size_t merge_sort_test(const std::vector<int>& input, const std::vector<int>& expected)
{
    std::vector<int> s(input);
    std::vector<int> buffer;
    buffer.assign(s.size(), 0);
    algo::merge_sort(s.begin(), s.end(), buffer.begin());
    return s == expected ? 0 : 1;
}

size_t validate_list(algo::slist<int>& l, int* v)
{
    for (algo::slist<int>::iterator it = l.begin(); it != l.end(); ++it)
    {
        if (*it != *v)
        {
            return 1;
        }
        ++v;
    }
    return 0;
}

void CASE_sort_basic()
{
    std::vector<int> seq;
    seq.reserve(100);

    size_t qs_failures = 0;
    size_t hs_failures = 0;
    size_t ms_failures = 0;
    for (int n = 1; n < 100; ++n)
    {
        seq.push_back(n);

        std::vector<int> shuffled_seq(seq);
        std::random_shuffle(shuffled_seq.begin(), shuffled_seq.end());

        qs_failures += quick_sort_test(shuffled_seq, seq);
        hs_failures += heap_sort_test(shuffled_seq, seq);
        ms_failures += merge_sort_test(shuffled_seq, seq);
    }

    if (qs_failures + hs_failures + ms_failures == 0)
    {
        std::cout << "CASE_sort_basic: PASSED\n";
    }
    else
    {
        std::cout << "CASE_sort_basic: FAILED!!!\n"
            << "    quick_sort failure " << qs_failures << '\n'
            << "    heap_sort  failure " << hs_failures << '\n'
            << "    merge_sort failure " << ms_failures << '\n';
    }
}

void CASE_sort_random()
{
    size_t qs_failures = 0;
    size_t hs_failures = 0;

    std::vector<int> randoms;
    for (int t = 0; t < 10; ++t)
    {
        generate_randoms(randoms);

        std::vector<int> expected(randoms);
        std::sort(expected.begin(), expected.end());

        qs_failures += quick_sort_test(randoms, expected);
        hs_failures += heap_sort_test(randoms, expected);

        // using customized predicator
        reversed_less pred;
        std::sort(expected.begin(), expected.end(), pred);

        qs_failures += quick_sort_pred_test(randoms, expected);
        hs_failures += heap_sort_pred_test(randoms, expected);
    }

    if (qs_failures + hs_failures == 0)
    {
        std::cout << "CASE_sort_random: PASSED\n";
    }
    else
    {
        std::cout << "CASE_sort_random: FAILED!!!\n";
        std::cout << "    quick_sort failure " << qs_failures << '\n'
            << "    heap_sort  failure " << hs_failures << '\n';
    }
}

void CASE_bsearch_basic()
{
    std::vector<int> seq;
    seq.reserve(100);
    for (int n = 0; n < 100; ++n)
    {
        seq.push_back(n);
    }

    size_t bs_failures = 0;
    for (int n = 0; n < 200; ++n)
    {
        if (n < 100 && !algo::binary_search(seq.begin(), seq.end(), n))
        {
            ++bs_failures;
        }
        else if (n >= 100 && algo::binary_search(seq.begin(), seq.end(), n))
        {
            ++bs_failures;
        }
    }

    std::cout << "CASE_bsearch_basic: " << (bs_failures == 0 ? "PASSED" : "FAILED!!!") << '\n';
}

void CASE_lubound_basic()
{
    std::vector<int> seq;
    seq.reserve((1 + 50) * 25);
    for (int n = 1; n < 51; ++n)
    {
        for (int c = 0; c < n; ++c)
        {
            seq.push_back(n);
        }
    }

    size_t lb_failures = 0;
    size_t ub_failures = 0;
    for (int n = 0; n < 55; ++n)
    {
        lb_failures += (algo::lower_bound(seq.begin(), seq.end(), n) != std::lower_bound(seq.begin(), seq.end(), n));
        ub_failures += (algo::upper_bound(seq.begin(), seq.end(), n) != std::upper_bound(seq.begin(), seq.end(), n));
    }

    if (lb_failures + ub_failures == 0)
    {
        std::cout << "CASE_lubound_basic: PASSED\n";
    }
    else
    {
        std::cout << "CASE_lubound_basic: FAILED!!!\n"
            << "    lower_bound failure " << lb_failures << '\n'
            << "    upper_bound failure " << ub_failures << '\n';
    }
}

void CASE_slist()
{
    size_t failures = 0;
    typedef algo::slist<int> list;

    {
        int v[1];
        list mylist;
        failures += validate_list(mylist, v);
    }

    {
        list mylist;
        mylist.push_back(1);
        mylist.push_back(2);
        mylist.push_back(3);
        mylist.push_back(4);
        int v[] = { 1, 2, 3, 4 };

        list cp(mylist);
        failures += validate_list(cp, v);

        cp.clear();
        failures += validate_list(cp, 0);
        failures += validate_list(mylist, v);
    }

    {
        algo::slist<int> mylist;
        mylist.push_back(3);
        mylist.push_front(2);
        mylist.push_back(4);
        mylist.push_front(1);
        int v[] = { 1, 2, 3, 4 };
        failures += validate_list(mylist, v);

        mylist.pop_back();
        mylist.pop_back();
        int v2[] = { 1, 2 };
        failures += validate_list(mylist, v2);

        mylist.pop_front();
        mylist.pop_front();
        int v3[1];
        failures += validate_list(mylist, v3);
    }

    {
        algo::slist<int> mylist;
        mylist.push_back(1);
        mylist.push_back(2);
        mylist.push_back(3);
        mylist.push_back(4);
        mylist.push_back(5);
        mylist.push_back(6);

        algo::slist<int>::iterator it = mylist.begin();
        ++it; ++it; ++it;  // *it == 4;
        failures += (*it == 4) ? 0 : 1;

        mylist.remove(it);
        int v1[] = { 1, 2, 3, 5, 6 };
        failures += validate_list(mylist, v1);

        mylist.insert(mylist.begin()++, 102); // insert 102 before 2
        mylist.insert(mylist.begin(), 0);
        mylist.insert(mylist.end(), 60);
        int v2[] = { 0, 1, 102, 2, 3, 5, 6, 60 };
        failures += validate_list(mylist, v2);
    }

    if (failures == 0)
    {
        std::cout << "CASE_slist: PASSED\n";
    }
    else
    {
        std::cout << "CASE_slist: FAILED!!!\n"
            << "    slist failure " << failures << '\n';
    }
}

void CASE_disjoint_set()
{
    unsigned prime[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47 };
    unsigned np = sizeof(prime) / sizeof(unsigned);

    // { {0}, {1}, ..., {99}}
    algo::disjoint_set  ds(100);

    for (unsigned i = 1; i < 100; ++i)
    {
        unsigned mfactor = 1;
        for (unsigned j = 0; j < np; ++j)
        {
            if (prime[j] > i)
            {
                break;
            }
            else if (!(i % prime[j]))
            {
                mfactor = prime[j];
            }
        }
        ds.join(i, mfactor);
    }

    size_t faults = 0;
    for (unsigned j = 0; j < np; ++j)
    {
        std::cout << "Max. prime factor " << prime[j] << ": ";
        for (unsigned i = 1; i < 100; ++i)
        {
            if (ds.connected(prime[j], i))
            {
                std::cout << i << ' ';
            }
        }
        std::cout << '\n';
    }
}

void CASE_linear_select()
{
    for (size_t n = 100; n < 1000000; n *= 10)
    {
        std::vector<int> inputs(n);
        for (size_t i = 0; i < n; ++i)
        {
            inputs[i] = i;
        }

        int m = algo::select(&(inputs[0]), n, n / 2);

        if (m != n / 2 - 1)
        {
            std::cout << "CASE_linear_select: FAILED!!!\n";
            return;
        }
    }
    std::cout << "CASE_linear_select: PASSED\n";
}
