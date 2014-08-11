#ifndef ALGORITHM_H
#define ALGORITHM_H

namespace algo
{
    template<typename T>
    void swap(T& t1, T& t2)
    {
        // trivial swap
        T tmp(t1);
        t1 = t2;
        t2 = tmp;
    }

    template<typename T>
    bool less_pred(const T& t1, const T& t2)
    {
        return t1 < t2;
    }

    template<typename T>
    class less
    {
    public:
        bool operator() (const T& t1, const T& t2) const
        {
            return algo::less_pred(t1, t2);
        }
    };

    // Sorts elements in range [first, last) into ascending order.
    // pred: predicator that defines a strict weak ordering
    template<
        typename RandomAccessIterator,
        typename Predicator>
    void quick_sort(RandomAccessIterator first,
                    RandomAccessIterator last,
                    Predicator           pred)
    {
        if( last - first < 2 )
        {
            // No need to take any action
            return;
        }

        // partition - divide and conquer
        RandomAccessIterator left = first, right = last;
        for(++left, --right; left < right; )
        {
            while( left < right && pred(*left, *first)   ){ ++left;  }  // here we got *left >= *pivot
            while( left < right && !pred(*right, *first) ){ --right; }  // here we got *right < *pivot

            if( left < right )
            {
                algo::swap(*left, *right);
                ++left;
                --right;
            }
        }

        // Move pivot element to its position
        if( !pred(*first, *right) )
        {
            algo::swap(*first, *right);
        }

        // At the end of loop, we got left >= right and distance(left, right) <= 1
        algo::quick_sort(first, right, pred);
        algo::quick_sort(left, last, pred);
    }
    
    template<typename RandomAccessIterator>
    void quick_sort(RandomAccessIterator first, RandomAccessIterator last)
    {
        if( first > last || last - first < 2 )
        {
            // No need to take any action
            return;
        }

        // partition - divide and conquer
        RandomAccessIterator left = first, right = last;
        for(++left, --right; left < right; )
        {
            while( left < right && *left < *first     ){ ++left;  }  // here we got *left >= *pivot
            while( left < right && !(*right < *first) ){ --right; }  // here we got *right < *pivot

            if( left < right )
            {
                algo::swap(*left, *right);
                ++left;
                --right;
            }
        }

        // At the end of loop, we got left >= right and distance(left, right) <= 1
        // Compare pivot element with the element at partition boundary
        if( !(*first < *right) )
        {
            algo::swap(*first, *right);
        }

        algo::quick_sort(first, right);
        algo::quick_sort(left, last);
    }

    template<
        typename RandomAccessIterator,
        typename Predicator>
    void _heap_sort_sift_down(RandomAccessIterator first,
                              size_t               start,
                              size_t               end,
                              Predicator           pred)
    {
        for(size_t root = start, child = 2*start+1; child < end; child = 2*root+1)
        {
            // select the top element from {root, left-child, right-child}
            size_t top = root;
            if( !pred(*(first+child), *(first+top)) )     // vs. left-child
            {
                top = child;
            }

            if( ++child < end )
            {
                if( !pred(*(first+child), *(first+top)) ) // vs. right-child
                {
                    top = child;
                }
            }

            if( top != root )
            {
                // put the top element to heap root
                algo::swap(*(first+top), *(first+root));

                // sift down the child
                root = top;
            }
            else break;
        }
    }


    template<typename RandomAccessIterator>
    void _heap_sort_sift_down(RandomAccessIterator first, size_t start, size_t end)
    {
        for(size_t root = start, child = 2*start+1; child < end; child = 2*root+1)
        {
            // select the top element from {root, left-child, right-child}
            size_t top = root;
            if( !(*(first+child) < *(first+top)) )     // vs. left-child
            {
                top = child;
            }
         
            if( ++child < end )
            {
                if( !(*(first+child) < *(first+top)) ) // vs. right-child
                {
                    top = child;
                }
            }

            if( top != root )
            {
                // put the top element to heap root
                algo::swap(*(first+top), *(first+root));

                // sift down the child
                root = top;
            }
            else break;
        }
    }

    // Sorts elements in range [first, last) into ascending order
    // pred: predicator that defines a strict weak ordering
    template<
        typename RandomAccessIterator,
        typename Predicator>
    void heap_sort(RandomAccessIterator first,
                   RandomAccessIterator last,
                   Predicator           pred)
    {
        size_t count = last - first;
        if( count < 2 )
        {
            return;
        }

        // constructs a max heap
        for(size_t h = (count - 2) / 2; h != 0; --h)
        {
            algo::_heap_sort_sift_down(first, h, count, pred);
        }
        algo::_heap_sort_sift_down(first, 0, count, pred);

        // pop top element out and adjust heap
        for(size_t n = count-1; n != 0; --n)
        {
            algo::swap(*first, *(first+n));
            algo::_heap_sort_sift_down(first, 0, n, pred);
        }
    }

    template<typename RandomAccessIterator>
    void heap_sort(RandomAccessIterator first, RandomAccessIterator last)
    {
        size_t count = last - first;
        if( count < 2 )
        {
            return;
        }

        // constructs a max heap
        for(size_t h = (count - 2) / 2; h != 0; --h)
        {
            algo::_heap_sort_sift_down(first, h, count);
        }
        algo::_heap_sort_sift_down(first, 0, count);

        for(size_t n = count-1; n != 0; --n)
        {
            algo::swap(*first, *(first+n));
            algo::_heap_sort_sift_down(first, 0, n);
        }
    }

    // Merge ordered sequence [first1, last1) and [first2, last2) into result.
    // Size of output buffer specified by result must be sufficient to store all
    // elements.
    template<
        typename InputIterator,
        typename OutputIterator,
        typename Predicator>
    void merge(InputIterator  first1, InputIterator last1,
               InputIterator  first2, InputIterator last2,
               OutputIterator result, Predicator    pred)
    {
        while( first1 != last1 && first2 != last2 )
        {
            if( pred(*first2, *first1) )
            {
                *result = *first2;
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
            }
            ++result;
        }

        for(; first1 != last1; ++first1, ++result)
        {
            *result = *first1;
        }

        for(; first2 != last2; ++first2, ++result)
        {
            *result = *first2;
        }
    }

    template<
        typename InputIterator,
        typename OutputIterator>
    void merge(InputIterator  first1, InputIterator last1,
               InputIterator  first2, InputIterator last2,
               OutputIterator result)
    {
        while( first1 != last1 && first2 != last2 )
        {
            if( *first2 < *first1 )
            {
                *result = *first2;
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
            }
            ++result;
        }

        for(; first1 != last1; ++first1, ++result)
        {
            *result = *first1;
        }

        for(; first2 != last2; ++first2, ++result)
        {
            *result = *first2;
        }
    }

    // Sorts elements in range [first, last) into ascending order.
    // buffer: Buffer to be used by the sorting procedure. Size of buffer should
    //  be at least number of elements in [first, last), i.e. last-first
    // pred:   predicator that defines a strict weak ordering
    template<typename RandomAccessIterator>
    void merge_sort(RandomAccessIterator first, RandomAccessIterator last,
                    RandomAccessIterator buffer)
    {
        size_t count = last - first;
        if( last < first || count <= 1 )
        {
            return;
        }

        // divide and conquer
        size_t half = count / 2;
        algo::merge_sort(first, first + half, buffer);
        algo::merge_sort(first + half, last, buffer + half);
        
        // merge subrange into buffer
        algo::merge(first, first+half, first+half, first+count, buffer);

        // in-place output, copy ordered elements from buffer to [first, last)
        algo::merge(buffer, buffer+count, last, last, first);
    }

    // Search for element within the ordered range [first, last)
    // pred: predicator that defines a strict weak ordering
    // Returns true if the specified element is found.
    template<
        typename RandomAccessIterator,
        typename Element,
        typename Predicator>
    bool binary_search(RandomAccessIterator first,
                       RandomAccessIterator last,
                       const Element&       element,
                       Predicator           pred)
    {
        while( first < last )
        {
            RandomAccessIterator center = first + (last-first)/2;
            if( pred(*center, element) )
            {
                first = center + 1; // target element may fall in [center+1, last)
            }
            else if( pred(element, *center) )
            {
                last = center;      // target element may fall in [first, center)
            }
            else return true;       // found it
        }
        return false;
    }

    template<
        typename RandomAccessIterator,
        typename Element>
    bool binary_search(RandomAccessIterator first,
                       RandomAccessIterator last,
                       const Element&       element)
    {
        return algo::binary_search(first, last, element, algo::less<Element>());
    }

    // Find the first element within [first, last) that isn't less than the specified value.
    // pred: predicator that defines a strict weak ordering
    template<
        typename RandomAccessIterator,
        typename Element,
        typename Predicator>
    RandomAccessIterator lower_bound(RandomAccessIterator first,
                                     RandomAccessIterator last,
                                     const Element&       value,
                                     Predicator           pred)
    {
        while( first < last )
        {
            RandomAccessIterator center = first + (last-first)/2;
            if( pred(*center, value) )
            {
                first = center + 1; // lower bound may fall in [center+1, last]
            }
            else // *center >= value
            {
                last = center;      // lower bound may fall in [first, center]
            }
        }
        return first;
    }

    template<
        typename RandomAccessIterator,
        typename Element>
    RandomAccessIterator lower_bound(RandomAccessIterator first,
                                     RandomAccessIterator last,
                                     const Element&       value)
    {
        return algo::lower_bound(first, last, value, algo::less<Element>());
    }

    // Find the first element within [first, last) that is greater than value.
    template<
        typename RandomAccessIterator,
        typename Element,
        typename Predicator>
    RandomAccessIterator upper_bound(RandomAccessIterator first,
                                     RandomAccessIterator last,
                                     const Element&       value,
                                     Predicator           pred)
    {
        while( first < last )
        {
            RandomAccessIterator center = first + (last-first)/2;
            if( pred(value, *center) )
            {
                last = center;      // upper bound may fall in [first, center]
            }
            else // value >= *center
            {
                first = center + 1; // upper bound may fall in [center+1, last]
            }
        }
        return last;
    }

    template<
        typename RandomAccessIterator,
        typename Element>
    RandomAccessIterator upper_bound(RandomAccessIterator first,
                                     RandomAccessIterator last,
                                     const Element&       value)
    {
        return algo::upper_bound(first, last, value, algo::less<Element>());
    }
}

#endif
