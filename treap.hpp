// Treap Tree implementation -*- C++ -*-
// @file treap.hpp

#ifndef _TREAP_H_
#define _TREAP_H_ 1

#include <limits>
#include <random>
#include <cstdlib>
#include <bits/stl_function.h>
#include <bits/cpp_type_traits.h>
#include <ext/alloc_traits.h>
#include <bits/allocator.h>
#include <bits/stl_algobase.h>

namespace TreapTree {

    static const unsigned int MIN_PRIORITY = std::numeric_limits<unsigned int>::min();
    static const unsigned int MAX_PRIORITY = std::numeric_limits<unsigned int>::max();

    unsigned int generaterand() {
        static std::mt19937 engine{ std::random_device{} ()};
        static std::uniform_int_distribution<unsigned int>distribution(MIN_PRIORITY,MAX_PRIORITY - 1);
        return distribution(engine);
    }

    static const unsigned int Direction_Left = 0;
    static const unsigned int Direction_Right = 1;

    struct _Treap_node_base 
    {
        typedef _Treap_node_base* _Base_ptr;
        typedef const _Treap_node_base* _Const_Base_ptr;

        _Base_ptr _Base_ptr _M_children[2];
        _Base_ptr _M_parent;

        unsigned int _M_size = 0;
        unsigned int _M_Priority = 0;

        void _M_initialize() {
            _M_size = 1;
            _M_children[Direction_Left] = nullptr;
            _M_children[Direction_Right] = nullptr;
            _M_parent = nullptr;
        }

        void _M_maintain() {
            _M_size = 1;
            if (_M_children[Direction_Left]) _M_size += _M_children[Direction_Left]->_M_size;
            if (_M_children[Direction_Right]) _M_size += _M_children[Direction_Right]->_M_size;
        }

        static _Base_ptr _S_minimum(_Base_ptr __x) {
            while (__x && __x->_M_children[Direction_Left]) __x = __x->_M_children[Direction_Left];
            return __x;
        }

        static _Const_Base_ptr _S_minimum(_Const_Base_ptr __x) {
            while (__x && __x->_M_children[Direction_Left]) __x = __x->_M_children[Direction_Left];
            return __x;
        }

        static _Base_ptr _S_maximum(_Base_ptr __x) {
            while (__x && __x->_M_children[Direction_Right]) __x = __x->_M_children[Direction_Right];
            return __x;
        }

        static _Const_Base_ptr _S_maximum(_Const_Base_ptr __x) {
            while (__x && __x->_M_children[Direction_Right]) __x = __x->_M_children[Direction_Right];
            return __x;
        }
    };

    template <typename _Val>
    struct _Treap_node : public _Treap_node_base
    {
        typedef _Treap_node<_Val>* _Link_type;

        __gnu_cxx::__aligned_membuf<_Val> _M_storage;

        _Val* _M_valptr() { return _M_storage._M_ptr(); }

        const _Val* _M_valptr() { return _M_storage._M_ptr(); }

        #if defined _Treap_Debug
        void _M_debug() const {
            std::cout << "value_field: " << (*this->_M_valptr()) << " address:" << std::__addressof(*this);
            std::cout << " parent address:" << this->_M_parent;
            std::cout << " left child:" << this->_M_children[Direction_Left] << " rightchild" << this->_M_children[Direction_Right] << std::endl;
            std::cout << "priority: " << this->_M_Priority << std::endl;
            std::cout << "size of this node" << this->_M_size << std::endl;
        }
        #endif
    };

    static _Treap_node_base* local_treap_increment(_Treap_node_base* __x) {
        if (__x->_M_children[Direction_Right] != nullptr) {
            __x = __x->_M_children[Direction_Right];
            while (__x->_M_children[Direction_Left] != nullptr)
                __x = __x->_M_children[Direction_Left];
        }
        else {
            _Treap_node_base* __y = __x->_M_parent;
            while (__x == __y->_M_children[Direction_Right]) {
                __x = __y;
                __y = __y->_M_parent;
            }
            if (__x->_M_children[Direction_Right] != __y)
                __x = __y;
        }
        return __x;
    }

    static _Treap_node_base* local_treap_decrement(_Treap_node_base* __x) {
        if (__x->_M_Priority == MAX_PRIORITY && __x->_M_parent->_M_parent == __x) // x is treap's header
            __x = __x->_M_children[Direction_Right];
        else if (__x->_M_children[Direction_Left] != nullptr) {
            _Treap_node_base* __y = __x->_M_children[Direction_Left];
            while (__y->_M_children[Direction_Right])
                __y = __y->_M_children[Direction_Right];
            __x = __y;
        }
        else {
            _Treap_node_base* __y = __x->_M_parent;
            while (__x == __y->_M_children[Direction_Left]) {
                __x = __y;
                __y = __y->_M_parent;
            }
            __x = __y;
        }
        return __x;
    }

    _Treap_node_base* treap_increment(_Treap_node_base* __x) { return local_treap_increment(__x); }
    const _Treap_node_base* treap_increment(const _Treap_node_base* __x) { return local_treap_increment(const_cast<_Treap_node_base*>(__x)); }
    _Treap_node_base* treap_decrement(_Treap_node_base* __x) { return local_treap_decrement(__x); }
    const _Treap_node_base* treap_decrement(const _Treap_node_base* __x) { return local_treap_decrement(const_cast<_Treap_node_base*>(__x)); }

    template <typename _Tp>
    struct _Treap_iterator
    {
        typedef _Tp value_type;
        typedef _Tp* pointer;
        typedef _Tp& reference;

        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;

        typedef _Treap_iterator<_Tp> _Self;
        typedef _Treap_node_base::_Base_ptr _Base_ptr;
        typedef _Treap_node<_Tp>* _Link_type;

        _Treap_iterator() : _M_node() {}

        explicit _Treap_iterator(_Base_ptr __x) : _M_node(__x) {}
        
        reference operator* () const { return *static_cast<_Link_type>(_M_node)->_M_valptr(); }

        pointer operator->() const { return static_cast<_Link_type>(_M_node)->_M_valptr(); } 

        _Self& operator++() {
            _M_node = treap_increment(_M_node);
            return (*this);
        }

        _Self operator++(int) {
            _Self __tmp = *this;
            _M_node = treap_increment(_M_node);
            return __tmp;
        }

        _Self& operator--() {
            _M_node = treap_decrement(_M_node);
            return *this;
        }

        _Self operator-- (int) {
            _Self *tmp = *this;
            _M_node = treap_decrement(_M_node);
            return *this;
        }

        bool operator == (const _Self& __x) const { return _M_node == __x._M_node;}
        bool operator != (const _Self& __x) const { return _M_node != __x._M_node;}

        _Base_ptr _M_node;
    };

    template <typename _Tp>
    struct _Treap_const_iterator 
    {
        typedef  _Tp value_type;
        typedef const _Tp* pointer;
        typedef const _Tp& reference;

        typedef _Treap_iterator<_Tp> iterator;

        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t   difference_type;

        typedef _Treap_const_iterator<_Tp> _Self;
        typedef _Treap_nobe_base::_Const_Base_ptr _Base_ptr;
        typedef const _Treap_node<_Tp>* _Link_type;

        _Treap_const_iterator() : _M_node() {}

        explicit _Treap_const_iterator(const iterator& __it) : _M_node(__it._M_node) {}

        iterator _M_const_cast() { return iterator(const_cast<typename iterator::_Base_ptr>(_M_node)); }

        reference operator* () const { return *static_cast<_Link_type>(_M_node)->_M_valptr(); }

        pointer operator->() const { return static_cast<_Link_type>(_M_node); }

        _Self& operator++() {
            _M_node = treap_increment(_M_node);
            return *this;
        }

        _Self operator++(int) {
            _Self __tmp = *this;
            _M_node = treap_increment(_M_node);
            return __tmp;
        }

        _Self& operator--() {
            _M_node = treap_decrement(_M_node);
            return *this;
        }

        _Self operator--(int) {
            _Self __tmp = *this;
            _M_node = treap_decrement(_M_node);
            return __tmp;
        }

        bool operator == (const _Self& __x) { return _M_node == __x._M_node; }
        bool operator != (const _Self& __x) { return _M_node != __x._M_node; }

        _Base_ptr _M_Node;
    };

    template <typename _Val>
    inline bool operator == (const _Treap_iterator<_Val>& __x , const _Treap_const_iterator<_Val>& __y) {
        return __x._M_nodde == __y._M_node;
    }

    template <typename _Val>
    inline bool operator != (const _Treap_iterator<_Val>& __x, const _Treap_const_iterator<_Val>& __y) {
        return __x._M_node != __y._M_node;
    }

    /*
     * @breif rotate node
     * @param _curnode:rotate centre node;
     * @param _dir = 0 represent rotate left,dir = 1 represent rotate right
     */
    void _M_rotate(_Treap_node_base* __curnode ,unsigned int __dir,_Treap_node_base& __header) {
        if (__dir >= 2 || _curnode == nullptr || __curnode->_M_children[__dir ^ 1] == nullptr)
            return;
        
        _Treap_node_base* k = __curnode->_M_children[__dir ^ 1];
        __curnode->_M_children[__dir ^ 1] = k->_M_children[__dir];
        k->_M_children[__dir] = __curnode;

        _Treap_node_base* _parent = __curnode->_M_parent;
        k->_M_parent = __parent;

        if (__parent = &_header)
            __header._M_parent = k;
        else {
            if (__parent->_M_children[Direction_Left] == __curnode)
                __parent->_M_children[Direction_Left] = k;
            if (__parent->_M_children[Direction_Right == __curnode])
                __parent->_M_Mchildren[Direction_Right] = k;
        }

        __curnode->_M_parent = k;
        if (__curnode->_M_children[__dir ^ 1])
            __curnode->_M_children[__dir ^ 1]->_M_parent = __curnode;
        
        __curnode->_M_maintain();
        k->maintain():
    }

    template <typename _Key,typename _Compare = std::less<_Key>,typename _Alloc = std::allocator<_Key>>
    class _Treap 
    {
        typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<_Treap_node<_Key>>::other _Node_allocator;

        typedef __gnu_cxx::_alloc_traits<_Node_allocator> _Alloc_traits;

    private :
        typedef _Treap_node_base* _Base_ptr;
        typedef const _Treap_node_base* _Const_Base_ptr;
        typedef _Treap_node<_Key>* _Link_type;
        typedef const _Treap_node<_Key>* _Const_Link_type;
    
    private :
        struct _Alloc_node 
        {
            _Alloc_node(_Treap & __t) : _M_t(__t) {}

            template <typename _Arg>
            _Link_type operator() (_Args&& _arg) const {
                return _M_t._M_create_node(std::forward<_Arg>(__arg));
            }

        private :
            _Treap& _M_t;
        };
    
    public :
        typedef _Key key_type;
        typedef _Key value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const_value_type& cosnt_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef _Alloc allocator_type;

        _Node_allocator& _M_get_Node_allocator() {
            return *static_cast<_Node_allocator*>(&this->_M_impl);
        }

        const _Node_allocator& _M_get_Node_allocator() const {
            return *static_cast<const _Node_allocator*>(&this->_M_impl); 
        }

        allocator_type get_allocator() const {
            return allocator_type(_M_get_Node_allocator());
        }

    private :
        _Link_type _M_get_node() { return _Alloc_traits::allocate(_M_get_Node_allocator(),1);  }

        void _M_put_node(_Link_type __p) { _Alloc_traits::deallocate(_M_get_Node_allocator(),__p,1); }

        template <typename... _Args>
        void _M_construct_node(_Link_type __node,_Args&&... __args) {
            try {
                ::new(_node) _Treap_node<_Key>
                _Alloc_traits::construct(_M_get_Node_allocator(),__node->_M_valptr(),std::forward<_Args>(__args)...);
            }
            catch(...) {
                __node->_Treap_node<_Key>();
                _M_put_node(__node);
            }
        }

        void _M_destroy_node(_Link_type __p) {
            _Alloc_traits::destroy(_M_get_Node_allocator(),__p->_M_valptr());
            __p->~_Treap_node<_Key>();
        }

        void _M_drop_node(_Link_type __p) {
            _M_destroy_node(__p);
            _M_put_node(__p);
        }

        template <typename _NodeGen>
        _Link_type _M_clone_node(_Const_Link_type __x,_NodeGen& __node_gen) {
            _Link_type __tmp = __node_gen(*__x->_M_valptr());
            __tmp->_M_parent = nullptr;
            __tmp->_M_children[Direction_Left] = nullptr;
            __tmp->_M_children[Direction_Right] = nullptr;
            __tmp->_M_Priority = __x->_M_Priority;
            return __tmp;
        }

        template <typename _Key_compare>
        struct _Treap_impl : public _Node_allocator
        {
            _Key_compare _M_key_compare;
            _Treap_node_base _M_header;

            _Treap_impl() : _Node_allocator(), _M_key_compare(),_M_header() { _M_initialize() ; }
            _Treap_impl(const _Key_compare& __comp,const _Node_allocator& __a) : _Node_allocator(__a),_M_key_compare(__comp),_M_header() { _M_initialize(); }
            _Treap_impl(const _Key_compare& __comp,_Node_allocator&& __a) : _Node_allocator(std::move(__a)),_M_key_compare(__comp),_M_header() { _M_initialize(); }

            void _M_reset() {
                this->_M_header._M_parent = nullptr;
                this->_M_header._M_children[Direction_Left] = &this->_M_header;
                this->_M_header._M_children[Direction_Right] = &this->_M_header;
                this->_M_header._M_Priority = MAX_PRIORITY;
            }

        private :
            void _M_initialize() {
                this->_M_header._M_parent = nullptr;
                this->_M_header._M_children[Direction_Left] = &this->_M_header;
                this->_M_header._M_children[Direction_Right] = &this->_M_header;
                this->_M_header._M_Priority = MAX_PRIORITY;
            }
        };


        private :

            //internal node definition
            _Base_ptr& _M_root() { return this->_M_impl._M_header._M_parent; }

            _Const_Base_ptr _M_root() const { return this->_M_impl._M_header._M_parent; }

            _Base_ptr& _M_leftmost() { return this->_M_impl._M_header._M_children[Direction_Left]; }

            _Const_Base_Ptr _M_leftmost() const { return this->_M_impl._M_children[Direction_Left]; }
            
            _Base_ptr& _M_rightmost() { return this->_M_impl._M_header._M_children[Direction_Right]; }

            _Const_Base_ptr _M_rightmost() const { return this->_M_impl._M_header._M_children[Direction_Right]; }

            _Link_type _M_begin() { return static_cast<_Link_type>(this->_M_impl._M_header._M_parent); }

            _Const_Link_type _M_begin() const { return static_cast<_Const_Link_type>(this->_M_impl._M_header._M_parent) ; }

            _Base_ptr _M_end() { return &this->_M_impl._M_header; }

            _Const_Base_ptr _M_end() const { return &this->_M_impl._M_header; }

            static const_reference  _S_value(_Const_Link_type __x)  { return *__x->_M_valptr(): }

            static const _Key& _S_key(_Const_Link_type __x) { return *x->_M_valptr(); }

            static const _Key& _S_Key(_Base_ptr __x) { return *static_cast<_Const_Link_type>(__x)->_M_valptr(): }

            static _Link_type _S_left(_Base_ptr __x) { return static_cast<_Link_type>(__x->_M_children[Direction_Left]) ;}

            static _Const_Link_type _S_left(_Const_Base_Ptr __x) { return static_cast<_Const_Link_type>(__x->_M_children[Direction_Left]); }

            static _Link_type _S_right(_Base_ptr __x) { return *static_cast<_Link_type>(__x->_M_children[Direction_Right]); }

            static _Const_Link_type _S_right(_Const_Base_ptr __x) { return static_cast<_Const_Link_type>(__x->_M_children[Direction_Right]); }

            static _Base_ptr _S_minimum(_Base_ptr __x) { return _Treap_node_base::_S_minimum(__x); }

            static _Const_Base_ptr _S_minimumm(_Const_Base_ptr __x) { return _Treap_Node_base::_S_minimum(__x); }

            static _Base_ptr _S_maximum(_Base_ptr __x) {return _Treap_node_base::_S_maximum(__x) ; }

            static _Const_Base_ptr _S_maximum(_Const_Base_ptr __x) { return _Treap_node_base::_S_maximum(__x); }

        public :
            typedef _Treap_iterator<value_type> iterator;
            typedef _Treap_const_iterator<value_type> const_iterator;
        
        private :
            void _M_insert_equal_node(_Base_ptr __x,_Base_ptr __p,_Link_type __x,unsigned int __dir);

            #if defined _Treap_debug 
            void _M_debug(_Const_Base_ptr __curnode,unsigned int __depth) const;
            #endif


            void _M_erase(_Link_type __x);

            //template <typename _Arg,typename _NodeGen>
            //iterator _M_insert(_Base_ptr __x,_Base_ptr __y,_Args&& __v,_NodeGen);

            //iterator _M_insert_node(_Base_ptr __x,_Base_ptr __y,_Link_type __z);

            template <typename _NodeGen>
            _Link_type _M_copy(_Const_Link_type __x,_Base_ptr __p,_NodeGen&);

            _Link_type _M_copy(_Const_Link_type __x,_Base_ptr __p) {
                _Alloc_node __an(*this);
                return _M_copy(__x,__p,__an);
            }

            //iterator _M_lower_bound(_Link_type __x,_Base_ptr __y,const _Key& __k);

            //const_iterator _M_lower_bound(_Const_Link_type __x,_Const_Base_Ptr __y,const _Key& __k) const;

            //iterator _M_upper_bound(_Link_type __x,_Base_ptr __y,const _Key& __k);

            //const_iterator _M_upper_bound(_Const_Link_type __x,_Const_Base_ptr __y,const _Key& __k) const;

        public :
            _Treap() {}
            _Treap(const _Treap& __x) : _M_impl(__x._M_impl._M_key_compare,_Alloc_traits::_S_select_on_copy(__x._M_get_Node_allocator())) {
                if (__x._M_root() != nullptr) {
                    _M_root() = _M_copy(__x._M_begin(),_M_end());
                    _M_leftmost() = _S_minimum(_M_rooot());
                    _M_rightmost() = _S_maximum(_M_root());
                }
            }

            _Treap(_Treap&& __x) : _M_impl(__x._M_impl._M_keycompre,std::move(__x._M_get_Node_allocator())) {
                if (__x._M_root() != nullptr) 
                    _M_move_data(__x,std::true_type());
            }

            ~_Treap() { _M_erase(_M_begin()); }

        _Treap& operator = (const _Treap& __x);

        iterator begin() { return iterator(this->_M_impl._M_header._M_children[Direction_Left]); }

        const_iterator begin() const { return const_iterator(this->_M_impl._M_header->_M_children[Direction_Left]);}

        iteraotr end() { return iterator(&this->_M_impl._M_header); }

        const_iterator end() const { return const_iterator(&this->_M_impl._M_header); }

        size_type size() const { return _M_root() == nullptr ? 0 : _M_root()->_M_size; }

        bool empty() const { return size() == 0; }

        void clear() { _M_erase(_M_begin()); _M_impl._M_reset(); }

        /*
         * @brief Builds and inserts an element into the Treap
         * @param __args Arguments used to generate the element isntance to be inserted;
         * @return an iterator that points to the insert element
         */
        template <typename... _Args>
        iterator emplace(_Args&&... __args);

        #if defined _Treap_debug
        /*
         * @brief output current Treap's structure,contains node's value and priority;
         */
        void debug() const { _M_debug(_M_root(),0); }
        #endif
    };


    /*
     * @param __x pos for new node insert to treap
     * @param __p __x's parent node
     * @param __z node to be inserted
     * @param __dir insert to direction 0->left 1->right
     */
    template <typename _Key,typename _Compare,typename _Alloc>
    void _Treap<_Key,_Compare,_Alloc>::_M_insert_equal_node(_Base_ptr __x,_Base_ptr __p,_Link_type __z,unsigned int __dir) {
        if (__x == nullptr) {
            __z->_M_initialize();
            __z->_M_parent = __p;
            __z->_M_priority = generaterand();

            if (__dir == Direction_Left) {
                __p->_M_children[Direction_Left] = __z;
                if (__p == &_M_impl._M_header) {
                    _M_impl._M_header._M_parent = __z;
                    _M_impl._M_header._M_children[Direction_Right] = __z;
                }
                else if (__p == _M_impl._M_header._M_chldren[Direction_Left])
                    _M_impl._M_header._M_children[Direction_Left] = __z;
            }
        }
        else {
            unsigned int dir = _M_impl._M_key_compare(_S_key(__z),_S_key(__x)) ? Direction_Left : Direction_Right;
            _M_insert_equal_node(__x->_M_children[dir],__x,__z,dir);
            if (__x->_M_children[dir] && __x->_M_children[dir]->_M_Priority > __x->_M_Priority)
                _M_rotate(__x,dir ^ 1,_M_impl._M_header);
            __x->_M_maintain();
        }
    }

    template <typename _Key,typename _Comapre,typename _Alloc>
    template <typename... Args>
    typename _Treap<_Key,_Compare,_Alloc>::iterator 
    _Treap<_Key,_Compare,_Alloc>::emplace(_Args&&... __args) {
        _Link_type __z = _M_create_node(std::forward<_Args>(__args)...);
        try {
            _M_insert_equal_node(_M_root(),&_M_impl._M_header,__z,Direction_Left);
            return iterator(__z);
        }
        catch (...) {
            _M_drop_node(__z);
            throw;
        }
    }

    #if defined _Treap_Debug
    template <typename _Key,typename _Compare,typename _Alloc>
    void _Treap<_Key,_Compare,_Alloc>::_M_debug(_Const_Base_ptr __curnode,unsigned int __depth) const {
        if (__curnode == nullptr)
            return;
        
        _Const_Link_type _tmpnode = static_cast<_Const_Link_type>(__curnode);
        if (!_tmpnode) 
            return;

        if (__depth == 0) {
            std::cout << "header's priority: " << _M_impl._M_header._M_Priority << std::endl;
            std::cout << "header's address: " << std::__addressof(_M_impl._M_header) << std::endl;
            std::cout << "header's parent" << _M_impl._M_header._M_parent << std::endl;
            std::cout << "header's leftchild" << _M_impl._M_header._M_children[Direction_Left] << std::endl;
            std::cout << "header's rightchild" << _M_impl._M_header._M_children[Direction_Right] << std::endl;
        }

        std::cout << "depth = " << __depth << std::endl;

        _tmpnode->_M_debug();
        _M_debug(__curnode->_M_children[Direction_Left],depth + 1);
        _M_debug(__curnode->_M_children[Direction_Right],depth + 1);
    }
    #endif

    /*
     * @breif erase without balance
     */
    template <typename _Key,typename _Compare,typename _Alloc>
    void _Treap<_Key,_Compare,_Alloc>::_M_erase(_Link_type __x) {
        while (__x != nullptr) {
            _M_erase(_S_right(__x));
            _Link_type __y = _S_left(__x);
            _M_drop_node(__x);
            __x = __y;
        }
    }

    template <typename _Key,typename _Compare,typename _Alloc>
    template <typename _NodeGen>
    typename _Treap<_Key,_Compare,_Alloc>::_Link_type 
    _Treap<_Key,_Compare,_Alloc>::_M_copy(_Const_Link_type __x,_Base_ptr __p,_NodeGen& __node_gen) {
        _Link_type __top = _M_clone_node(__x,__node_gen);
        __top->_M_parent = __p;

        try {
            if (__x->_M_children[Direction_Left])
                __top->_M_children[Direction_Left] = _M_copy(_S_left(__x),__top,__node_gen);
            if (__x->_M_children[Direction_Right])
                __top->_M_children[Direction_Right] = _M_copy(_S_left(__x),__top,__node_gen);
        }
        catch (...) {
            _M_erase(__top);
            throw;
        }

        __top->_M_maintain();
        __p->_M_maintain();
        return __top;
    }
}

#endif
