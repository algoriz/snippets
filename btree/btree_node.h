#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include "btree_helper.h"

namespace algo
{
    template<typename K, typename V, size_t Order>
    class btree;

    template<typename K, typename V, size_t Order>
    class btree_node
        : public std::enable_shared_from_this<btree_node<K, V, Order> >
    {
    public:
        typedef btree_node<K, V, Order>                 my_type;
        typedef std::pair<K, V>                         value_type;
        typedef K                                       key_type;
        typedef std::shared_ptr<my_type>                shptr;
        typedef std::weak_ptr<my_type>                  wkptr;
        typedef std::vector<value_type>                 keyvalue_v;
        typedef std::vector<shptr>                      subtree_v;
        typedef typename keyvalue_v::iterator           key_iterator;
        typedef typename subtree_v::iterator            tree_iterator;
        typedef btree_helper::btree_order_limits<Order> limits;

        btree_node(): _selfpos(-1) {}

        // retrieve std::shared_ptr version of this pointer
        shptr         thisptr()                     { return shared_from_this(); }

        // get/set pointer to parent node, nullptr for root node
        shptr         get_parent() const            { return _parent.lock(); }
        void          set_parent(shptr parent)      { _parent = parent; }

        // get/set the position of current node
        size_t        get_selfpos() const           { return _selfpos; }
        void          set_selfpos(size_t p)         { _selfpos = p; }

        // keys stored in current node
        keyvalue_v&   key()                         { return _keyvalues; }

        // subtrees
        subtree_v&    sub()                         { return _subtrees;  }

        // key iteration facilities
        key_iterator  first_key()                   { return _keyvalues.begin(); }
        key_iterator  last_key()                    { return _keyvalues.end(); }

        // subtree iteration facilities
        tree_iterator first_child()                 { return _subtrees.begin(); }
        tree_iterator last_child()                  { return _subtrees.end(); }

        // left side sibling node, which has the same parent with current node
        shptr left_sibling() const
        {
            shptr lsibling;
            if( !is_root() && _selfpos != 0 )
            {
                shptr pa = get_parent();
                lsibling = pa->sub()[_selfpos-1];
            }
            return lsibling;
        }

        // right side sibling node, which has the same parent with current node
        shptr right_sibling() const
        {
            shptr rsibling;
            if( !is_root() )
            {
                shptr pa = get_parent();
                if( pa->sub().size() > _selfpos+1 )
                {
                    rsibling = pa->sub()[_selfpos+1];
                }
            }
            return rsibling;
        }

        // Tests whether current node is leaf node
        bool is_leaf() const { return _subtrees.empty(); }

        // Tests whether current node is root node
        bool is_root() const { return _selfpos == -1; }

        // Number of keys stored in current node
        size_t key_count() const { return _keyvalues.size(); }

        void insert(const value_type& val);

        void remove(const value_type& val);

        void swap(my_type& another);

        key_iterator locate(const value_type& val)
        {
            return std::lower_bound(first_key(), last_key(), val, kvcomp());
        }

    private:
        typedef btree_helper::compare<K, V> kvcomp;

        void insert_key(size_t p, const value_type& val);
        void insert_child(size_t p, shptr node);
        void erase_key_at(size_t p)                 { btree_helper::erase(_keyvalues, p); }
        void erase_child_at(size_t p);
        void erase_key_from(size_t p)               { btree_helper::erase_from(_keyvalues, p); }
        void update_subtree(size_t p = 0);
        void split();
        bool rotate_left();
        bool rotate_right();

        // pop max key and rebalance
        void pop_max_key(value_type& m);

        // pop min key and rebalance
        void pop_min_key(value_type& m);

        // Merges n-th subtree and (n+1)-th subtree of current node
        void merge(size_t n);

        // Remove the n-th key from current node
        // removing a key should be in company with removing a child
        void remove_n(size_t n);

        // Rebalance the tree starting from current node
        void rebalance();

        template<typename, typename, size_t>
        friend class btree;

    private:
        wkptr      _parent;
        size_t     _selfpos;
        keyvalue_v _keyvalues;
        subtree_v  _subtrees;
    };

    template<typename K, typename V, size_t Order>
    bool btree_node<K, V, Order>::rotate_left()
    {
        shptr rsibling(nullptr);
        shptr parent = get_parent();
        if( parent && parent->key_count() > _selfpos )
        {
            rsibling = parent->sub()[_selfpos+1];
        }

        if( !rsibling || rsibling->key_count() <= limits::key_lower )
        {
            // no enough element for rotation
            return false;
        }

        // separator from parent
        value_type& s = parent->key()[_selfpos];

        value_type tmp;
        key().push_back(tmp);
        btree_helper::swap(key().back(), s);
        btree_helper::swap(rsibling->key().front(), s);
        rsibling->erase_key_at(0);
        if( !rsibling->is_leaf() )
        {
            shptr rsibling_sub = rsibling->sub()[0];
            rsibling->erase_child_at(0);
            insert_child(sub().size(), rsibling_sub);
        }

        rsibling->rebalance();
        return true;
    }

    template<typename K, typename V, size_t Order>
    bool btree_node<K, V, Order>::rotate_right()
    {
        shptr lsibling(nullptr);
        shptr parent = get_parent();
        if( parent && _selfpos > 0 )
        {
            lsibling = parent->sub()[_selfpos-1];
        }

        if( !lsibling || lsibling->key_count() <= limits::key_lower )
        {
            // no enough element for rotation
            return false;
        }

        value_type& s = parent->key()[_selfpos-1];

        value_type tmp;
        insert_key(0, tmp);
        btree_helper::swap(key().front(), s);
        btree_helper::swap(lsibling->key().back(), s);
        lsibling->key().pop_back();
        if( !lsibling->is_leaf() )
        {
            shptr lsibling_sub = lsibling->sub().back();
            lsibling->sub().pop_back();
            insert_child(0, lsibling_sub);
        }

        lsibling->rebalance();
        return true;
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::insert_key(size_t p, const value_type& val)
    {
        btree_helper::insert(_keyvalues, p, val);
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::insert_child(size_t p, shptr node)
    {
        btree_helper::insert(_subtrees, p, node);
        node->set_parent(thisptr());

        size_t count = _subtrees.size();
        for(; p < count; ++p)
        {
            _subtrees[p]->set_selfpos(p);
        }
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::erase_child_at(size_t p)
    {
        btree_helper::erase(_subtrees, p);

        size_t count = _subtrees.size();
        for(; p < count; ++p)
        {
            _subtrees[p]->set_selfpos(p);
        }
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::update_subtree(size_t p)
    {
        size_t count = _subtrees.size();
        for(; p < count; ++p)
        {
            _subtrees[p]->set_parent(thisptr());
            _subtrees[p]->set_selfpos(p);
        }
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::insert(const value_type& val)
    {
        size_t count = _keyvalues.size();
        size_t lb = 0;
        for(; lb < count; ++lb)
        {
            if( kvcomp::less(val, _keyvalues[lb]) )
            {
                break;
            }
            else if( !kvcomp::less(_keyvalues[lb], val) )
            {
                // in case the key already exists, just overwrite it
                _keyvalues[lb].second = val.second;
                return;
            }
        }

        // insertion always occurs in a leaf node
        // if current node is not a leaf node, step into subtree
        if( !is_leaf() )
        {
            return _subtrees[lb]->insert(val);
        }

        // This is a leaf node, insert val
        insert_key(lb, val);

        // split current node, if needed
        split();
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::remove(const value_type& val)
    {
        size_t count = _keyvalues.size();
        size_t lb = 0;
        for(; lb < count; ++lb)
        {
            if( kvcomp::less(val, _keyvalues[lb]) )
            {
                break;
            }
            else if( !kvcomp::less(_keyvalues[lb], val) )
            {
                return thisptr()->remove_n(lb);
            }
        }

        // if current node is not a leaf node, step into child node
        // otherwise, val is not found
        if( !is_leaf() )
        {
            return sub()[lb]->remove(val);
        }
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::swap(my_type& another)
    {
        std::swap(_parent, another._parent);
        std::swap(_selfpos, another._selfpos);
        std::swap(_keyvalues, another._keyvalues);
        std::swap(_subtrees, another._subtrees);
        update_subtree();
        another.update_subtree();
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::split()
    {
        if( key_count() < limits::key_upper )
        {
            return; // no need to split
        }

        size_t break_pos = key_count() / 2;
        
        // get median
        // median in _keyvalues is deleted later to avoid unnecessary move
        value_type median;
        btree_helper::swap(_keyvalues[break_pos], median);

        // copy values after median to a new node
        shptr parent = get_parent();
        shptr rchild(new my_type());
        rchild->set_parent(parent);
        rchild->set_selfpos(_selfpos);
        btree_helper::move(_keyvalues, break_pos+1, rchild->_keyvalues);
        btree_helper::move(_subtrees, break_pos+1, rchild->_subtrees);
        rchild->update_subtree();

        // delete dummy median place holder
        _keyvalues.pop_back();

        if( is_root() )
        {
            // another new node is needed
            shptr lchild(new my_type());
            btree_helper::move(_keyvalues, 0, lchild->_keyvalues);
            btree_helper::move(_subtrees, 0, lchild->_subtrees);
            lchild->update_subtree();

            insert_key(0, median);
            insert_child(0, lchild);
            insert_child(1, rchild);
            return;
        }

        parent->insert_key(_selfpos, median);
        parent->insert_child(_selfpos+1, rchild);
        parent->split();
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::remove_n(size_t n)
    {
        // If current node is a leaf node, just delete the key and rebalance the tree
        if( is_leaf() )
        {
            erase_key_at(n);
            rebalance();
            return;
        }

        // Otherwise, shift the deletion to the right most leaf of left side subtree
        // Or
        //_subtrees[n]->move_max_to(_keyvalues[n]);
        _subtrees[n+1]->pop_min_key(_keyvalues[n]);
        
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::pop_min_key(value_type& m)
    {
        shptr p = thisptr();
        while( !p->is_leaf() )
        {
            p = p->sub().front();
        }

        btree_helper::swap(p->key().front(), m);
        p->erase_key_at(0);
        p->rebalance();
    }
    
    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::pop_max_key(value_type& m)
    {
        shptr p = thisptr();
        while( !p->is_leaf() )
        {
            p = p->sub().back();
        }

        btree_helper::swap(p->key().back(), m);
        p->key().pop_back();
        p->rebalance();
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::merge(size_t n)
    {
        shptr lsub = sub()[n];
        shptr rsub = sub()[n+1];
    
        lsub->key().push_back(key()[n]);
        btree_helper::cut_paste_using_swap(
            rsub->key(), rsub->first_key(), rsub->last_key(),
            lsub->key(), lsub->last_key());
    
        size_t roffset = lsub->sub().size();
        btree_helper::cut_paste_using_swap(
            rsub->sub(), rsub->first_child(), rsub->last_child(),
            lsub->sub(), lsub->last_child());
        lsub->update_subtree(roffset);
    
        erase_key_at(n);
        erase_child_at(n+1);
    }

    template<typename K, typename V, size_t Order>
    void btree_node<K, V, Order>::rebalance()
    {
        if( key_count() >= limits::key_lower || is_root() )
        {
            return;
        }

        // Try to balance current node by rotation
        if( rotate_left() || rotate_right() )
        {
            return;
        }

        // If current node has a right sibling than merge current node with right
        // sibling. Otherwise merge current node with left sibling.
        // A non-root node has at least a sibling.
        shptr parent = get_parent();
        parent->merge(_selfpos < parent->key_count() ? _selfpos : _selfpos-1);
        
        // The parent loses a key, thus maybe deficient
        if( parent->is_root() && !parent->key_count() )
        {
            shptr self = thisptr();
            parent->sub().clear();
            parent->swap(*self);
            parent->_selfpos = -1;
            parent->_parent.reset();
            return;
        }

        parent->rebalance();
    }

}
