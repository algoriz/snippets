#pragma once
#include "btree_node.h"

namespace algo
{
    template<typename, typename, size_t>
    class btree;

    // naive forward iterator
    template<typename K, typename V, size_t Order>
    class btree_iterator
    {
    private:
        typedef typename btree_node<K,V,Order>        node_type;
        typedef typename btree_node<K,V,Order>::shptr node_ptr;
    
    public:
        typedef typename node_type::value_type        value_type;
        typedef btree_iterator<K,V,Order>             my_type;

        btree_iterator(): _g(-1), _ptr(nullptr) {}

        value_type& operator* () { return _ptr->key()[_g]; }
        value_type* operator->() { return &(_ptr->key()[_g]); }

        btree_iterator& operator++()
        {
            if( !_ptr->is_leaf() )
            {
                // if this is not leaf then we are back from a leaf
                // thus here g < key_count()
                _ptr = _ptr->sub()[_g+1];
                _g = 0;

                // Go down to first element of next subtree
                while( !_ptr->is_leaf() )
                {
                    _ptr = _ptr->sub()[0];
                }
            }
            else
            {
                ++_g;

                // Go up, if we have finished visiting current node
                while( _ptr && _ptr->key_count() <= _g )
                {
                    _g = _ptr->get_selfpos();
                    _ptr = _ptr->get_parent();
                }
            }
            return *this;
        }

        bool operator== (const my_type& another) const
        {
            return _ptr == another._ptr && _g == another._g;
        }

        bool operator!= (const my_type& another) const
        {
            return !(*this == another);
        }

    private:
        template<typename, typename, size_t>
        friend class btree;

        btree_iterator(node_ptr p, size_t g): _ptr(p), _g(g){}

        node_ptr _ptr;
        size_t   _g;
    };

    // memory b-tree
    template<typename K, typename V, size_t Order>
    class btree
    {
    public:
        typedef btree<K, V, Order>              my_type;
        typedef btree_node<K, V, Order>         node_type;
        typedef typename node_type::value_type  value_type;
        typedef typename node_type::key_type    key_type;
        typedef btree_iterator<K, V, Order>     iterator;

        btree(): _root(new node_type())    {}

        // insert a key-value pair into the tree
        void insert(const value_type& val) { _root->insert(val); }

        // erase a key-value pair from the tree
        void erase(const key_type& k)
        {
            value_type tmp;
            tmp.first = k;
            _root->remove(tmp);
        }

        // tests whether the tree is empty, i.e. the tree contains no any keys
        bool empty() const { return !_root->key_count(); }

        iterator begin();
        iterator end() { return iterator(); }
        iterator find(const key_type& k);

    private:
        typedef typename node_type::shptr         node_ptr;
        typedef typename node_type::key_iterator  key_iterator;
        typedef typename node_type::tree_iterator tree_iterator;
        typedef typename node_type::kvcomp        kvcomp;

    private:
        node_ptr _root;
    };

    template<typename K, typename V, size_t Order>
    typename btree<K, V, Order>::iterator btree<K, V, Order>::begin()
    {
        if( !_root->key_count() )
        {
            return iterator();
        }

        node_ptr p = _root;
        while( !p->is_leaf() )
        {
            p = p->sub()[0];
        }

        return iterator(p, 0);
    }

    template<typename K, typename V, size_t Order>
    typename btree<K, V, Order>::iterator btree<K, V, Order>::find(const key_type& k)
    {
        if( empty() )
        {
            return end();
        }

        value_type val;
        val.first = k;

        size_t ip = 0;
        key_iterator kit;
        for(node_ptr p = _root; p->key_count(); p = p->sub()[ip])
        {
            kit = p->locate(val);
            ip = kit - p->first_key();
            if( kit != p->last_key() && kvcomp::equal(val, *kit) )
            {
                return iterator(p, ip);
            }

            if( p->is_leaf() ){ break; }
        }
        return end();
    }
}
