#ifndef SLIST_H
#define SLIST_H
#include <memory>
#include <stdexcept>

namespace algo
{
    // single list
    template<typename ElementType>
    class slist
    {
    private:
        typedef slist<ElementType> my_type;

        class NodeType
        {
        public:
            NodeType(): next(0) {}
            
            NodeType(ElementType e): next(0), element(e) {}

            ~NodeType()
            {
                if( next ){ delete next; next = 0; }
            }

            ElementType element;
            NodeType*   next;
        };
        
        // iterator NON-STANDARD implementation
        class IteratorType
        {
        public:
            typedef ElementType value_type;

            IteratorType(): _ptr(0){}

            ElementType& operator*()  const { return _ptr->element; }
            
            IteratorType  operator++(int) const { return IteratorType(_ptr->next); }
            IteratorType& operator++()          { _ptr = _ptr->next; return *this; }

            bool operator== (const IteratorType& another) const
            {
                return _ptr == another._ptr;
            }

            bool operator!= (const IteratorType& another) const
            {
                return !(*this == another);
            }

        private:
            friend class my_type;
            IteratorType(NodeType* ptr): _ptr(ptr){}

        private:
            NodeType* _ptr;
        };

        class ConstIteratorType
        {
        public:
            typedef const ElementType value_type;

            ConstIteratorType(): _ptr(0){}

            value_type& operator*()  const { return _ptr->element; }

            ConstIteratorType   operator++(int) const { return ConstIteratorType(_ptr->next); }
            ConstIteratorType&  operator++()          { _ptr = _ptr->next; return *this; }

            bool operator== (const ConstIteratorType& another) const
            {
                return _ptr == another._ptr;
            }

            bool operator!= (const ConstIteratorType& another) const
            {
                return !(*this == another);
            }

        private:
            friend class my_type;
            ConstIteratorType(const NodeType* ptr): _ptr(ptr){}

        private:
            const NodeType* _ptr;
        };
    public:
        typedef IteratorType iterator;
        typedef ConstIteratorType const_iterator;

        slist(): _front(0), _back(0){}

        // copy constructor
        slist(const my_type& another): _front(0), _back(0) { _copy(another); }

        ~slist(){ clear(); }

        my_type& operator = (const my_type& another)       { _copy(another); }

        iterator  begin()       { return iterator(_front); }
        iterator  end()         { return iterator(0); }

        const_iterator  begin() const { return const_iterator(_front); }
        const_iterator  end()   const { return const_iterator(0); }

        bool      empty() const { return _front == 0; }
        NodeType& front()       { return *_front; }
        NodeType& back()        { return *_back; }

        // Add an element to the front of single list. Needs O(1) time.
        void push_front(ElementType element)
        {
            NodeType* ep = new NodeType(element);

            if( !_front )
            {
                _front = ep;
                _back  = ep;
            }
            else
            {
                ep->next = _front;
                _front = ep;
            }
        }

        // Add an element to the rear of single list. Needs O(1) time.
        void push_back(ElementType element)
        {
            NodeType* ep = new NodeType(element);
            
            if( !_back )
            {
                _front = ep;
                _back  = ep;
            }
            else
            {
                _back->next = ep;
                _back = ep;
            }
        }

        // Insert an element at position. Insertion at random position needs O(n) time.
        void insert(iterator position, ElementType element)
        {
            NodeType* target = position._ptr;
            if( !target || !_back ) // insertion at rear or the list is empty
            {
                push_back(element);
                return;
            }

            if( target == _front )
            {
                push_front(element);
                return;
            }

            if( NodeType* prior = _prior(target) )
            {
                NodeType* ep = new NodeType(element);
                ep->next = target;
                prior->next = ep;
            }
            else { throw std::invalid_argument("Bad position."); }
        }

        // Remove an element. Removal at random position needs O(n) time.
        void remove(iterator position)
        {
            NodeType* target = position._ptr;
            if( target == _front )
            {
                return pop_front();
            }

            NodeType* prior;
            if( !target || !(prior = _prior(target)) )
            {
                throw std::invalid_argument("Invalid position.");
            }

            prior->next = target->next;
            target->next = 0;

            if( _back == target )
            {
                _back = prior;
            }
            delete target;
        }

        // Pop an element from the front. Needs O(1) time.
        void pop_front()
        {
            if( !_front )
            {
                throw std::out_of_range("empty list.");
            }
            
            if( _back == _front )
            {
                _back = 0;
            }
            
            NodeType* pop = _front;
            _front = pop->next;
            pop->next = 0;
            delete pop;
        }

        // Pop an element from the rear. Needs O(n) time.
        void pop_back()
        {
            if( !_back )
            {
                throw std::out_of_range("empty list.");
            }

            if( _front == _back )
            {
                _front = 0;
            }

            NodeType* prior = _prior(_back);
            delete _back;
            _back = prior;
            _back->next = 0;
        }

        // Remove all elements.
        void clear()
        {
            delete _front;
            _front = 0;
            _back = 0;
        }

    private:
        // find the prior of p
        NodeType* _prior(NodeType* p)
        {
            if( p == _front )
            {
                return 0;
            }

            NodeType* prior = _front;
            while( prior && prior->next != p )
            {
                prior = prior->next;
            }
            return prior;
        }
        
        void _copy(const my_type& another)
        {
            clear();
            for(my_type::const_iterator it = another.begin(); it != another.end(); ++it)
            {
                push_back(*it);
            }
        }

        NodeType* _front;
        NodeType* _back;
    };
}

#endif
