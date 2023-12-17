#ifndef VECTOR_H
#define VECTOR_H
#include "normal_iterator.h"
#include <memory>
#include <initializer_list>

namespace lcf {
    template<typename T, typename Alloc>
    struct VectorBase {
        using alloc_type = Alloc;
        using alloc_traits = std::allocator_traits<alloc_type>;
        using pointer = typename alloc_traits::pointer;
        using value_type = T;
        VectorBase() { impl.start = impl.finish = impl.end_of_storage = nullptr; }
        VectorBase(size_t n) { create_storage(n); }
        ~VectorBase() { deallocate(impl.start, impl.end_of_storage - impl.start); }
        pointer allocate(size_t n) { return n == 0 ? pointer(nullptr) : alloc_traits::allocate(impl, n); }
        void deallocate(pointer p, size_t n) { if (p != nullptr) { alloc_traits::deallocate(impl, p, n); } }
        void create_storage(size_t n) { impl.finish = impl.start = allocate(n); impl.end_of_storage = impl.start + n; }
        struct VectorImpl : Alloc { pointer start, finish, end_of_storage; };
        VectorImpl impl;
    };

    template<typename T, typename Alloc = std::allocator<T>>
    class vector : protected VectorBase<T, Alloc> {
        using _Base = VectorBase<T, Alloc>;
        using _Self = vector<T, Alloc>;
        using alloc_traits = typename _Base::alloc_traits;
    public:
        using value_type = T;
        using referece = value_type&;
        using const_reference = const value_type&;
        using pointer = typename _Base::pointer;
        using const_pointer = typename alloc_traits::const_pointer;
        using size_type = size_t;
        using iterator = lcf::normal_iterator<pointer>;
        using const_iterator = lcf::normal_iterator<const_pointer>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        vector() = default;
        vector(size_t n) : _Base(n) { this->default_initialize(n); }
        vector(size_t n, const_reference val) : _Base(n) { this->fill_initialize(n, val); }
        vector(const _Self& other) : _Base(std::size(other)) { this->range_initialize(std::begin(other), std::end(other)); }
        vector(const std::initializer_list<value_type>& list) : _Base(list.size()) { this->range_initialize(list.begin(), list.end()); }
        template<typename ForwarIterator, typename _ = decltype(*std::declval<ForwarIterator>())>
        vector(ForwarIterator first, ForwarIterator last) : _Base(std::distance(first, last)) { this->range_initialize(first, last); }
        ~vector() { std::destroy(this->impl.start, this->impl.finish); }
        _Self& operator=(const _Self& other) {
            if (this == std::addressof(other)) { return *this; }
            if (other.size() > this->size()) { this->create_storage(other.size()); }
            this->range_initialize(other.cbegin(), other.cend());
            return *this;
        }
        //*--------------------------Iterators--------------------------*//
        iterator begin() { return iterator(this->impl.start); }
        const_iterator begin() const { return const_iterator(this->impl.start); }
        iterator end() { return iterator(this->impl.finish); }
        const_iterator end() const { return const_iterator(this->impl.finish); }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        //*--------------------------Capacity---------------------------*//
        bool empty() const { return this->impl.start == this->impl.finish; }
        size_type size() const { return this->impl.finish - this->impl.start; }
        size_type capacity() const { return this->impl.end_of_storage - this->impl.start; }
        void reserve(size_t size) { reallocate(size); }
        //*--------------------------Modifiers--------------------------*//
        void clear() { std::destroy(this->impl.start, this->impl.finish); this->impl.finish = this->impl.start; }
        void insert(size_t insert_pos, size_t n, const_reference val) { _insert(insert_pos, n, val); }
        void insert(size_t insert_pos, const_reference val) { _insert(insert_pos, 1, val); }
        template<typename ForwardIterator>
        void insert(size_t insert_pos, ForwardIterator first, ForwardIterator last)
        { for (; first != last; ++first) { _insert(insert_pos++, 1, *first); } }
        void insert(size_t insert_pos, const std::initializer_list<value_type> list)
        { this->insert(insert_pos, list.begin(), list.end()); }
        void push_back(const_reference val) { _insert(this->size(), 1, val); }
        void pop_back() { std::destroy_at(this->impl.finish); --this->impl.finish; }
        void erase(size_t first, size_t last) {
            std::destroy(this->impl.start + first, this->impl.start + last);
            std::copy(this->impl.start + last, this->impl.finish, this->impl.start + first);
            this->impl.finish -= last - first;
        }
        void erase(size_t pos) { this->erase(pos, pos + 1); }
        void swap(_Self& other) {  std::swap(this->impl, other.impl); }
        //*-----------------------Element Access----------------------*//
        referece operator[](size_type index) { return *(this->impl.start + index); }
        referece at(size_type index) {
            if (index >= size()) { throw std::out_of_range("out of range"); }
            return this->operator[](index);
        }
        const_reference operator[](size_type index) const { return *(this->impl.start + index); }
        referece front() { return *(this->impl.start); }
        const_reference front() const { return *(this->impl.start); }
        referece back() { return *(this->impl.finish - 1); }
        const_reference back() const { return *(this->impl.finish - 1); }
    protected:
        void default_initialize(size_t n) { this->impl.finish = std::uninitialized_default_construct_n(this->impl.start, n); }
        void fill_initialize(size_t n, const value_type& val) { this->impl.finish = std::uninitialized_fill_n(this->impl.start, n, val); }
        template<typename ForwardIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last)
        { this->impl.finish = std::uninitialized_copy(first, last, this->impl.start); }
        bool check_extra_size(size_t extra_size) { return this->impl.end_of_storage - this->impl.finish >= extra_size; }
        size_t new_size(size_t extra_size) { return capacity() + std::max(capacity(), extra_size); }
        void reallocate_insert(size_t extra_size, size_t insert_pos) {
            size_t old_size = size(), new_size = this->new_size(extra_size);
            pointer old_start = this->impl.start, old_finish = this->impl.finish;
            this->create_storage(new_size);
            this->impl.finish = this->impl.start + old_size + extra_size;
            std::copy(old_start, old_start + insert_pos, this->impl.start);
            std::copy_backward(old_start + insert_pos, old_finish, this->impl.finish);
            std::destroy(old_start, old_finish);
            this->deallocate(old_start, old_size);
        }
        void reallocate(size_t n) {
            if (n <= capacity()) { return; }
            pointer old_start = this->impl.start, old_finish = this->impl.finish;
            this->create_storage(n);
            this->impl.finish = this->impl.start + (old_finish - old_start);
            if (old_start != old_finish) {
                std::copy(old_start, old_finish, this->impl.start);
                std::destroy(old_start, old_finish);
                this->deallocate(old_start, old_finish - old_start);
            }
        }
        void _insert(size_t insert_pos, size_t n, const_reference val) {
            if (insert_pos > this->size()) { return; }
            if (check_extra_size(n)) {
                std::copy_backward(this->impl.start + insert_pos, this->impl.finish, this->impl.finish + n);
                std::advance(this->impl.finish, n);
            } else { reallocate_insert(n, insert_pos); }
            std::uninitialized_fill_n(this->impl.start + insert_pos, n, val);
        }
    };
}

#endif