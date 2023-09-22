#ifndef MARTIX_ITERATOR_H
#define MARTIX_ITERATOR_H
#include "iterator.h"

namespace lcf {
    template<typename T>
    class MatrixIterator : public lcf::Iterator<std::random_access_iterator_tag, T> {
    public:
        MatrixIterator(T* ptr) : _base(ptr) {}
        MatrixIterator(const MatrixIterator& other) : _base(other._base) {}
        MatrixIterator& operator++() {++_base; return *this;}
        MatrixIterator operator++(int) {MatrixIterator tmp(*this); ++_base; return tmp;}
        MatrixIterator& operator--() {--_base; return *this;}
        MatrixIterator operator--(int) {MatrixIterator tmp(*this); --_base; return tmp;}
        bool operator<(const MatrixIterator& rhs) const {return _base < rhs._base;}
        bool operator<=(const MatrixIterator& rhs) const {return _base <= rhs._base;}
        bool operator>=(const MatrixIterator& rhs) const {return _base >= rhs._base;}
        bool operator>(const MatrixIterator& rhs) const {return _base > rhs._base;}
        bool operator==(const MatrixIterator& rhs) const {return _base == rhs._base;}
        bool operator!=(const MatrixIterator& rhs) const {return _base != rhs._base;}
        T& operator*() {return *_base;}
    private:
        T* _base;
    };

    template<typename T>
    class MatrixRowIterator : public lcf::Iterator<std::random_access_iterator_tag, T> {
    public:
        MatrixRowIterator(T* ptr, ptrdiff_t row_size, ptrdiff_t col_size) :
            _base(ptr), _row_size(row_size), _column_size(col_size), _row_index(0), _column_index(0)
        { }
        MatrixRowIterator& operator++() {
            if (_row_index == _row_size - 1 && _column_index == _column_size - 1) {++_base; return *this;}
            if (_row_index < _row_size - 1) {
                _base += _column_size;
                ++_row_index;
            } else {
                ++(_base -= (_row_size - 1) * _column_size);
                ++_column_index;
                _row_index = 0;
            }
            return *this;
        }
        MatrixRowIterator operator++(int) {MatrixRowIterator tmp(*this); this->operator++(); return tmp;}
        bool operator<(const MatrixRowIterator& rhs) const {return _base < rhs._base;}
        bool operator<=(const MatrixRowIterator& rhs) const {return _base <= rhs._base;}
        bool operator>=(const MatrixRowIterator& rhs) const {return _base >= rhs._base;}
        bool operator>(const MatrixRowIterator& rhs) const {return _base > rhs._base;}
        bool operator==(const MatrixRowIterator& rhs) const {return _base == rhs._base;}
        bool operator!=(const MatrixRowIterator& rhs) const {return _base != rhs._base;}
        T& operator*() {return *_base;}
    private:
        T* _base;
        const ptrdiff_t _row_size;
        const ptrdiff_t _column_size;
        ptrdiff_t _row_index;
        ptrdiff_t _column_index;
    };
}

#endif