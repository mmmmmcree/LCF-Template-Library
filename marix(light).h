#include <vector>

namespace lcf {
    template<typename T>
    class matrix : public std::vector<T> {
        using _Base = std::vector<T>;
        using _Self = matrix<T>;
    public:
        using value_type = typename _Base::value_type;
        using reference = typename _Base::reference;
        using const_reference = typename _Base::const_reference;
        using size_type = typename _Base::size_type;
        matrix(size_type row_size, size_type col_size, const_reference value = value_type{})
        : _Base(row_size * col_size, value), _row_size(row_size), _col_size(col_size) { }
        size_type row_size() const { return _row_size; }
        size_type col_size() const { return _col_size; }
        inline __attribute__((__always_inline__))
        reference operator()(size_type row_idx, size_type col_idx) noexcept
        { return this->_M_impl._M_start[row_idx * _col_size + col_idx]; }
        inline __attribute__((__always_inline__))
        const_reference operator()(size_type row_idx, size_type col_idx) const noexcept
        { return this->_M_impl._M_start[row_idx * _col_size + col_idx]; }
        inline __attribute__((__always_inline__))
        reference operator()(const std::pair<int, int>& coord) noexcept
        { return this->operator()(coord.first, coord.second); }
        inline __attribute__((__always_inline__))
        const_reference operator()(const std::pair<int, int>& coord) const noexcept
        { return this->operator()(coord.first, coord.second); }
    private:  
        size_type _row_size, _col_size;
    };
}

namespace lcf {
    template<typename T>
    class square_matrix : public std::vector<T> {
        using _Base = std::vector<T>;
        using _Self = square_matrix<T>;
    public:
        using value_type = typename _Base::value_type;
        using reference = typename _Base::reference;
        using const_reference = typename _Base::const_reference;
        using size_type = typename _Base::size_type;
        square_matrix(size_type size, const_reference value = value_type{})
        : _Base(size * size, value), _size(size) { }
        size_type dimension() const { return _size; }
        inline __attribute__((__always_inline__))
        reference operator()(size_type row_idx, size_type col_idx) noexcept
        { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
        inline __attribute__((__always_inline__))
        const_reference operator()(size_type row_idx, size_type col_idx) const noexcept
        { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
        inline __attribute__((__always_inline__))
        reference operator()(const std::pair<int, int>& coord) noexcept
        { return this->operator()(coord.first, coord.second); }
        inline __attribute__((__always_inline__))
        const_reference operator()(const std::pair<int, int>& coord) const noexcept
        { return this->operator()(coord.first, coord.second); }
    private:  
        size_type _size;
    };
}