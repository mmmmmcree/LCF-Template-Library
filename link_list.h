#ifndef LINK_LIST_H
#define LINK_LIST_H
#include <iterator>
#include <initializer_list>

namespace lcf {
    struct BidirectionalNodeBase {
        using _Self = BidirectionalNodeBase;
        BidirectionalNodeBase(_Self* _prev = nullptr, _Self* _next = nullptr) : prev(_prev), next(_next)
        {
            if (prev == nullptr) { prev = this; }
            else { prev->next = this; }
            if (next == nullptr) { next = this; }
            else { next->prev = this; }
        }
        ~BidirectionalNodeBase() {
            next->prev = prev;
            prev->next = next;
        }
        bool single() const { return prev == this and next == this; }
        _Self *prev, *next;
    };

    template <typename T>
    struct Node : BidirectionalNodeBase {
        using _Base = BidirectionalNodeBase;
        Node(const T& _data, _Base* prev, _Base* next) : data(_data), _Base(prev, next) { }
        T data;
    };

    template <typename T>
    class circular_linklist_iterator {
        using _Self = circular_linklist_iterator<T>;
        using _BasePointer = std::conditional_t<std::is_const_v<T>, const BidirectionalNodeBase*, BidirectionalNodeBase*>;
        using _Pointer = std::conditional_t<std::is_const_v<T>, const Node<T>*, Node<T>*>;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
        circular_linklist_iterator(_BasePointer base, _BasePointer dummy) : _base(base), _dummy(dummy) { }
        _Pointer base() { return static_cast<_Pointer>(_base); }
        _Self& operator++() { _base = (_base->next == _dummy ? _dummy->next : _base->next); return *this; }
        _Self operator++(int) {
            _Self tmp = *this;
            _base = (_base->next == _dummy ? _dummy->next : _base->next);
            return tmp;
        }
        _Self& operator--() { _base = (_base->prev == _dummy ? _dummy->prev : _base->prev); return *this; }
        _Self operator--(int) {
            _Self tmp = *this; 
            _base = (_base->prev == _dummy ? _dummy->prev : _base->prev);
            return tmp;
        }
        reference operator*() const { return static_cast<_Pointer>(_base)->data; }
        pointer operator->() const { return &(static_cast<_Pointer>(_base)->data); }
        friend bool operator==(const _Self& lhs, const _Self& rhs) { return lhs._base == rhs._base; }
        friend bool operator!=(const _Self& lhs, const _Self& rhs) { return lhs._base != rhs._base; }
    private:
        _BasePointer _base, _dummy;
    };

    template <typename T>
    class linklist_iterator {
        using _Self = linklist_iterator<T>;
        using _BasePointer = std::conditional_t<std::is_const_v<T>, const BidirectionalNodeBase*, BidirectionalNodeBase*>;
        using _Pointer = std::conditional_t<std::is_const_v<T>, const Node<T>*, Node<T>*>;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
        linklist_iterator(_BasePointer base) : _base(base) { }
        linklist_iterator(circular_linklist_iterator<T> iter) : _base(iter.base()) { }
        _Pointer base() { return static_cast<_Pointer>(_base); }
        _Self& operator++() { _base = _base->next; return *this; }
        _Self operator++(int) { _Self tmp = *this; _base = _base->next; return tmp; }
        _Self& operator--() { _base = _base->prev; return *this; }
        _Self operator--(int) { _Self tmp = *this;  _base = _base->prev; return tmp; }
        reference operator*() const { return static_cast<_Pointer>(_base)->data; }
        pointer operator->() const { return &(static_cast<_Pointer>(_base)->data); }
        friend bool operator==(const _Self& lhs, const _Self& rhs) { return lhs._base == rhs._base; }
        friend bool operator!=(const _Self& lhs, const _Self& rhs) { return lhs._base != rhs._base; }
    private:
        _BasePointer _base;
    };

    template <typename T>
    class link_list {
        using node_type = Node<T>;
        using _Self = link_list<T>;
    public:
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using circular_iterator = circular_linklist_iterator<value_type>;
        using const_circular_iterator = circular_linklist_iterator<const value_type>;
        using iterator = linklist_iterator<value_type>;
        using const_iterator = linklist_iterator<const value_type>;
        link_list() = default;
        template <typename ForwardIterator>
        link_list(ForwardIterator first, ForwardIterator last)
        { for (; first != last; ++first) { push_back(*first); } }
        link_list(const std::initializer_list<value_type>& init_list)
        : link_list(init_list.begin(), init_list.end()) { }
        link_list(const _Self& other) : link_list(other.begin(), other.end()) { }
        ~link_list() { clear(); }
        void push_back(const_reference val) { new node_type(val, head.prev, &head); }
        void push_front(const_reference val) { new node_type(val, &head, head.next); }
        void pop_front() { delete static_cast<node_type*>(head.next); } // 不做异常处理
        void pop_back() { delete static_cast<node_type*>(head.prev); }
        void erase(iterator iter) {
            if (head.single()) { return; }
            delete iter.base();
        }
        void insert(iterator iter, const_reference val)
        { new node_type(val, iter.base()->prev, iter.base()); }
        void clear() { while (not head.single()) { pop_back(); } }
        iterator begin() { return iterator(head.next); }
        const_iterator begin() const { return const_iterator(head.next); }
        iterator end() { return iterator(&head); }
        const_iterator end() const { return const_iterator(&head); }
        circular_iterator circular_begin() { return circular_iterator(head.next, &head); }
        const_circular_iterator circular_begin() const { return const_circular_iterator(head.next, &head); }
        circular_iterator circular_before_end() { return circular_iterator(head.prev, &head); }
        const_circular_iterator circular_before_end() const { return circular_iterator(head.prev, &head); }
        bool empty() const { return head.single(); }
        _Self& operator=(const _Self& other) {
            if (this == &other) { return *this; }
            auto first = other.begin(), last = other.end();
            auto begin = this->begin(), end = this->end();
            for (; first != last and begin != end; ++first, ++begin) { *begin = *first; }
            for (; first != last; ++first) { this->push_back(*first); }
            while (end != begin) { --end; this->pop_back(); }
            return *this;
        }
    private:
        typename node_type::_Base head;
    };
}

#endif