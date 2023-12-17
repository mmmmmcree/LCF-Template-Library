#ifndef NORMAL_ITERATOR_H
#define NORMAL_ITERATOR_H
#include <iterator>

namespace lcf {
    template<typename Iterator>
    class normal_iterator {
        using _Self = normal_iterator<Iterator>;
    public:
        using traits_type = std::iterator_traits<Iterator>;
        using iterator_category = typename traits_type::iterator_category;
        using value_type = typename traits_type::value_type;
        using difference_type = typename traits_type::difference_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;
        normal_iterator(Iterator base) : _base(base) { }
        difference_type operator-(const _Self& rhs) const { return _base - rhs._base; }
        _Self& operator++() { ++_base; return *this; }
        _Self operator++(int) { return _Self(_base++); }
        _Self& operator--() { --_base; return *this; }
        _Self operator--(int) { return _Self(_base--); }
        _Self operator+(difference_type offset) { return _Self(_base + offset); }
        _Self operator-(difference_type offset) { return _Self(_base - offset); }
        _Self& operator+=(difference_type offset) { _base += offset; return *this; }
        _Self& operator-=(difference_type offset) { _base -= offset; return *this; }
        reference operator*() const { return *_base; }
        pointer operator->() const { return _base; }
        reference operator[](difference_type index) const { return _base[index]; }
        pointer base() const { return _base; }
        friend bool operator==(const _Self& lhs, const _Self& rhs) { return lhs._base == rhs._base; }
        friend bool operator!=(const _Self& lhs, const _Self& rhs) { return lhs._base != rhs._base; }
    private:
        Iterator _base;
    };
}

#endif