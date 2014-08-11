#include "disjoint_set.h"
using namespace algo;


algo::disjoint_set::disjoint_set(unsigned count)
{
    _set.reserve(count);
    for(unsigned i = 0; i < count; ++i)
    {
        _set.push_back(i);
    }
}

void algo::disjoint_set::join(unsigned set1, unsigned set2)
{
    unsigned pa1 = find(set1);
    unsigned pa2 = find(set2);
    if( pa1 != pa2 )
    {
        // merge set#1 into set#2
        _set[pa1] = pa2;
    }
}

unsigned algo::disjoint_set::find(unsigned element)
{
    _range_check(element);

    unsigned pa = _set[element];
    if( pa != element )
    {
        pa = (_set[element] = find(pa));
    }
    return pa;
}

bool algo::disjoint_set::connected(unsigned element1, unsigned element2)
{
    return find(element1) == find(element2);
}

void algo::disjoint_set::reset()
{
    size_t count = _set.size();
    for(unsigned i = 0; i < count; ++i)
    {
        _set[i] = i;
    }
}

void algo::disjoint_set::_range_check(unsigned n)
{
    if( n >= _set.size() )
    {
        throw std::out_of_range("disjoint_set out_of_range error");
    }
}
