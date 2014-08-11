#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include <vector>

namespace algo
{
    class disjoint_set
    {
    public:
        // Creates a disjoint set that contains count elements.
        // Elements are identified by their unique zero-based index.
        explicit disjoint_set(unsigned count);

        // Join two subsets
        // set1 and set2 are representative elements of the subsets to be joint.
        void     join(unsigned set1, unsigned set2);

        // Determine which subset the specified element is in.
        unsigned find(unsigned element);
    
        // Tests whether the specified two elements are in the same subset.
        bool     connected(unsigned element1, unsigned element2);

        // Divide all subsets into atoms
        void     reset();
    
    private:
        void _range_check(unsigned n);

        std::vector<unsigned> _set;
    };
}

#endif