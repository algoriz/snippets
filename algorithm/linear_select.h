#pragma once
#include <vector>

namespace algo
{
    template<typename T>
    void insert_sort(T* seq, size_t length)
    {
        for(size_t i = 1; i < length; ++i)
        {
            if( seq[i] < seq[i-1] )
            {
                size_t j = i;
                T t = seq[j];
                while( t < seq[j-1] && j > 0 )
                {
                    seq[j] = seq[j-1];
                    --j;
                }
                seq[j] = t;
            }
        }
    }

    template<typename T>
    T middle_of(T* seq, size_t length)
    {
        insert_sort(seq, length);
        return seq[ (length-1)/2 ];
    }

    // Partition sequence
    // Return number of elements in lower part.
    template<typename T>
    size_t partition(T* seq, size_t length, T pivot)
    {
        if( length <= 1 )
        {
            return 0;
        }

        T* l = seq;
        T* r = l + length - 1;
        while( l < r )
        {
            while( *l < pivot && l < r ){ ++l; }
            while( *r > pivot && l < r ){ --r; }

            if( l < r )
            {
                T t = *l;
                *l = *r;
                *r = t;
                ++l;
                --r;
            }
        }

        return l - seq;
    }

    // Select the Nth minimal element from the sequence
    template<typename T>
    T select(T* seq, size_t length, size_t n)
    {
        if( length <= n )
        {
            return seq[length-1];
        }

        size_t groups = (length+4) / 5;
        std::vector<T> mids;
        for(size_t i = 0; i < groups - 1; ++i)
        {
            mids.push_back( algo::middle_of(seq + 5*i, 5) );
        }

        mids.push_back(algo::middle_of(seq + 5 * (groups - 1), length % 5));
        T midmids = algo::middle_of(&(mids[0]), mids.size());

        size_t k = algo::partition(seq, length, midmids) + 1;
        if( n == k )
        {
            return seq[k-1];
        }
        else if( n < k )
        {
            return algo::select(seq, k - 1, n);
        }
        return algo::select(seq + k, length - k, n - k);
    }
}
