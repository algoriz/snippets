#pragma once

namespace btree_helper
{
    template<typename K, typename V>
    class compare
    {
    public:
        typedef std::pair<K,V> value_type;

        bool operator() (const value_type& lv, const value_type& rv) const
        {
            return less(lv, rv);
        }

        static bool less(const value_type& lv, const value_type& rv)
        {
            static std::less<K> pred;
            return pred(lv.first, rv.first);
        }

        static bool equal(const value_type& lv, const value_type& rv)
        {
            return !less(lv, rv) && !less(rv, lv);
        }
    };

    // Limits on a btree node
    // #key should be in range [key_lower, key_upper)
    // #sub should be in range [sub_lower, key_upper)
    template<size_t Order>
    class btree_order_limits
    {
    public:
        enum
        {
            key_lower = Order / 2,
            key_upper = Order,
            sub_lower = key_lower + 1,
            sub_upper = key_upper + 1,
        };
    };

    // See btree google code for why this swap
    template<typename T>
    void swap(T& l, T& r)
    {
        using std::swap;
        swap(l, r);
    }

    template<typename Vector, typename K>
    void insert(Vector& v, size_t p, const K& k)
    {
        v.insert(v.begin()+p, k);
    }

    template<typename Vector, typename Iterator>
    void cut_paste_using_swap(Vector& src, Iterator src_begin, Iterator src_end, Vector& dst, Iterator pos)
    {
        size_t count = src_end - src_begin;
        size_t offset = pos - dst.begin();

        typename Vector::value_type tmp;
        dst.insert(pos, count, tmp);

        Iterator p = dst.begin() + offset;
        for(Iterator g = src_begin; g != src_end; ++g, ++p)
        {
            btree_helper::swap(*g, *p);
        }
        src.erase(src_begin, src_end);
    }

    template<typename Vector>
    void erase(Vector& v, size_t p)
    {
        v.erase(v.begin()+p);
    }

    template<typename Vector>
    void erase_from(Vector& v, size_t p)
    {
        v.erase(v.begin()+p, v.end());
    }

    // Move src[p, ) to dst[0, )
    template<typename Vector>
    void move(Vector& src, size_t p, Vector& dst)
    {
        if( src.empty() )
        {
            return;
        }

        size_t count = src.size() - p;

        typename Vector::value_type tmp;
        dst.clear();
        dst.assign(count, tmp);

        for(size_t i = p, j = 0; j < count; ++i, ++j)
        {
            btree_helper::swap(src[i], dst[j]);
        }
        btree_helper::erase_from(src, p);
    }
}
