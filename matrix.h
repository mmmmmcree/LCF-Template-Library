#ifndef MATRIX_H
#define MATRIX_H
#include <vector>

namespace lcf {
    template<typename T, typename Alloc = std::allocator<T>>
    class matrix : private std::vector<T, Alloc> {
        using _Base = std::vector<T, Alloc>;
        using _Self = matrix<T, Alloc>;
    public:
        using value_type = typename _Base::value_type;
        using pointer = typename _Base::pointer;
        using const_pointer = typename _Base::const_pointer;
        using reference = typename _Base::reference;
        using const_reference = typename _Base::const_reference;
        using iterator = typename _Base::iterator;
        using size_type = typename _Base::size_type;
        //*----------------------------------------------------------------------------*//
        matrix(size_type row_size, size_type col_size, const_reference value = value_type{})
        : _Base(row_size * col_size, value), _row_size(row_size), _col_size(col_size) { }
        iterator begin() { return _Base::begin(); }
        iterator end() { return _Base::end(); }
        size_type size() const { return _Base::size(); }
        pointer data() { return _Base::data(); }
        const_pointer data() const { return _Base::data(); }
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
        { return this->_M_impl._M_start[coord.first * _col_size + coord.second]; }
        inline __attribute__((__always_inline__))
        const_reference operator()(const std::pair<int, int>& coord) const noexcept
        { return this->_M_impl._M_start[coord.first * _col_size + coord.second]; }
        _Self& operator=(const std::initializer_list<value_type>& init_list) { this->assign(init_list); return *this; }
        _Self& set_value(const value_type& value) { this->assign(this->size(), value_type{}); return *this; }
        //*----------------------------------------------------------------------------*//
        friend _Self operator+(const _Self& lhs, const _Self& rhs) {
            size_type row_size = lhs.row_size() & rhs.row_size(), col_size = lhs.col_size() & rhs.col_size();
            _Self result(row_size, col_size);
            for (size_type i = 0, n = result.size(); i < n; ++i) { result[i] = lhs[i] + rhs[i]; }
            return result;
        }
        friend _Self& operator+=(_Self& lhs, const _Self& rhs)
        { for (size_type i = 0, n = lhs.size(); i < n; ++i) { lhs[i] += rhs[i]; } return lhs; }
        friend _Self operator-(const _Self& lhs, const _Self& rhs) {
            size_type row_size = lhs.row_size() & rhs.row_size(), col_size = lhs.col_size() & rhs.col_size();
            _Self result(row_size, col_size);
            for (size_type i = 0, n = result.size(); i < n; ++i) { result[i] = lhs[i] - rhs[i]; }
            return result;
        }
        friend _Self& operator-=(_Self& lhs, const _Self& rhs)
        { for (size_type i = 0, n = lhs.size(); i < n; ++i) { lhs[i] -= rhs[i]; } return lhs; }
        friend _Self operator*(const _Self& lhs, const _Self& rhs) {
            size_type row_size = lhs.row_size() & rhs.col_size(), col_size = lhs.col_size() & rhs.row_size();
            _Self result(row_size, row_size);
            for (size_type i = 0; i < row_size; ++i) {
                for (size_type j = 0; j < row_size; ++j) {
                    for (size_type k = 0; k < col_size; ++k)
                    { result(i, j) += lhs(i, k) * rhs(k, j); }
                }
            } return result;
        }
        friend _Self& operator*=(_Self& lhs, const _Self& rhs) { return lhs = lhs * rhs; }
        template<typename Merge = std::multiplies<value_type>, typename Update = std::plus<value_type>>
        friend _Self multiply(const _Self& lhs, const _Self& rhs, Merge merge = Merge(), Update update = Update(), value_type init_value = value_type()) {
            size_type row_size = lhs.row_size() & rhs.col_size(), col_size = lhs.col_size() & rhs.row_size();
            _Self result(row_size, row_size, init_value);
            for (size_type i = 0; i < row_size; ++i) {
                for (size_type j = 0; j < row_size; ++j) {
                    for (size_type k = 0; k < col_size; ++k)
                    { result(i, j) = update(result(i, j), merge(lhs(i, k),  rhs(k, j))); }
                }
            } return result;
        }
        //*----------------------------------------------------------------------------*//
        friend std::ostream& operator<<(std::ostream& os, const _Self& matrix) {
            for (int i = 0, n = matrix.row_size(); i < n; ++i) {
                for (int j = 0, m = matrix.col_size(); j < m; ++j)
                { os << matrix(i, j) << ' '; } os << std::endl;
            } return os;
        }
        friend std::istream& operator>>(std::istream& is, _Self& matrix) {
            for (int i = 0, n = matrix.row_size(); i < n; ++i) {
                for (int j = 0, m = matrix.col_size(); j < m; ++j) { is >> matrix(i, j); }
            } return is;
        }
    private:  
        size_type _row_size, _col_size;
    };
}

namespace lcf {
    template<typename T, typename Alloc = std::allocator<T>>
    class square_matrix : private std::vector<T, Alloc> {
        using _Base = std::vector<T, Alloc>;
        using _Self = square_matrix<T, Alloc>;
    public:
        using value_type = typename _Base::value_type;
        using pointer = typename _Base::pointer;
        using const_pointer = typename _Base::const_pointer;
        using reference = typename _Base::reference;
        using const_reference = typename _Base::const_reference;
        using iterator = typename _Base::iterator;
        using size_type = typename _Base::size_type;
        //*----------------------------------------------------------------------------*//
        square_matrix(size_type size, const_reference value = value_type{}) : _Base(size * size, value), _size(size) { }
        iterator begin() { return _Base::begin(); }
        iterator begin() const { return _Base::begin(); }
        iterator end() { return _Base::end(); }
        iterator end() const { return _Base::end(); }
        size_type size() const { return _Base::size(); }
        pointer data() { return _Base::data(); }
        const_pointer data() const { return _Base::data(); }
        inline __attribute__((__always_inline__))
        reference operator()(size_type row_idx, size_type col_idx) noexcept
        { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
        inline __attribute__((__always_inline__))
        const_reference operator()(size_type row_idx, size_type col_idx) const noexcept
        { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
        inline __attribute__((__always_inline__))
        reference operator()(const std::pair<int, int>& coord) noexcept
        { return this->_M_impl._M_start[coord.first * _size + coord.second]; }
        inline __attribute__((__always_inline__))
        const_reference operator()(const std::pair<int, int>& coord) const noexcept
        { return this->_M_impl._M_start[coord.first * _size + coord.second]; }
        size_type dimension() const { return _size; }
        _Self& normalize(const value_type& value) { for (size_type i = 0; i < _size; ++i) { this->operator[](i * _size + i) = value; }; return *this; }
        _Self& set_value(const value_type& value) { this->assign(this->size(), value_type{}); return *this; }
        _Self& operator=(const std::initializer_list<value_type>& init_list) { this->assign(init_list); return *this; }
        //*----------------------------------------------------------------------------*//
        friend _Self operator+(const _Self& lhs, const _Self& rhs) {
            size_type dimension = lhs.dimension() & rhs.dimension();
            _Self result(dimension);
            for (size_type i = 0, n = result.size(); i < n; ++i) { result[i] = lhs[i] + rhs[i]; }
            return result;
        }
        friend _Self& operator+=(_Self& lhs, const _Self& rhs)
        { for (size_type i = 0, n = lhs.size(); i < n; ++i) { lhs[i] += rhs[i]; } return lhs; }
        friend _Self operator-(const _Self& lhs, const _Self& rhs) {
            size_type dimension = lhs.dimension() & rhs.dimension();
            _Self result(dimension);
            for (size_type i = 0, n = result.size(); i < n; ++i) { result[i] = lhs[i] - rhs[i]; }
            return result;
        }
        friend _Self& operator-=(_Self& lhs, const _Self& rhs)
        { for (size_type i = 0, n = lhs.size(); i < n; ++i) { lhs[i] -= rhs[i]; } return lhs; }
        friend _Self operator*(const _Self& lhs, const _Self& rhs) {
            size_type dimension = lhs.dimension() & rhs.dimension();
            _Self result(dimension);
            for (size_type i = 0; i < dimension; ++i) {
                for (size_type j = 0; j < dimension; ++j) {
                    for (size_type k = 0; k < dimension; ++k)
                    { result(i, j) += lhs(i, k) * rhs(k, j); }
                }
            } return result;
        }
        friend _Self& operator*=(_Self& lhs, const _Self& rhs) { return lhs = lhs * rhs; }
        friend _Self& operator^=(_Self& base, size_t exponent) {
            if (exponent-- == 0) { return base.set_value(0).normalize(1); }
            for (_Self product(base); exponent > 0; exponent >>= 1, product = product * product) {
                if (exponent & 1) { base = base * product; }
            } return base;
        }
        friend _Self operator^(_Self base, size_t exponent) { return base ^= exponent; }
        template<typename Merge = std::multiplies<value_type>, typename Update = std::plus<value_type>>
        friend _Self multiply(const _Self& lhs, const _Self& rhs, Merge merge = Merge(), Update update = Update(), value_type init_value = value_type()) {
            size_type dimension = lhs.dimension() & rhs.dimension();
            _Self result(dimension, init_value);
            for (size_type i = 0; i < dimension; ++i) {
                for (size_type j = 0; j < dimension; ++j) {
                    for (size_type k = 0; k < dimension; ++k)
                    { result(i, j) = update(result(i, j), merge(lhs(i, k),  rhs(k, j))); }
                }
            } return result;
        }
        template<typename Merge = std::multiplies<value_type>, typename Update = std::plus<value_type>>
        friend _Self pow(_Self base, size_t exponent, Merge merge = Merge(), Update update = Update(), value_type init_value = value_type()) {
            if (exponent-- == 0) { return base; }
            for (_Self product(base); exponent > 0; exponent >>= 1, product = multiply(product, product, merge, update, init_value)) {
                if (exponent & 1) { base = multiply(base, product, merge, update, init_value); }
            } return base;
        }
        //*----------------------------------------------------------------------------*//
        friend std::ostream& operator<<(std::ostream& os, const _Self& matrix) {
            for (int i = 0, n = matrix.dimension(); i < n; ++i) {
                for (int j = 0, m = matrix.dimension(); j < m; ++j)
                { os << matrix(i, j) << ' '; } os << std::endl;
            } return os;
        }
        friend std::istream& operator>>(std::istream& is, _Self& matrix) {
            for (int i = 0, n = matrix.dimension(); i < n; ++i) {
                for (int j = 0, m = matrix.dimension(); j < m; ++j) { is >> matrix(i, j); }
            } return is;
        }
    private:  
        size_type _size;
    };
}

#endif

// namespace lcf {
//     template<typename T, typename Alloc = std::allocator<T>>
//     class matrix : private std::vector<T, Alloc> {
//     public:
//         using _Base = std::vector<T, Alloc>;
//         using _Self = matrix<T, Alloc>;
//         using value_type = typename _Base::value_type;
//         using reference = typename _Base::reference;
//         using const_reference = typename _Base::const_reference;
//         using iterator = typename _Base::iterator;
//         using size_type = typename _Base::size_type;
//         matrix(size_type row_size, size_type col_size, const_reference value = value_type{})
//         : _Base(row_size * col_size, value), _row_size(row_size), _col_size(col_size) { }
//         iterator begin() { return _Base::begin(); }
//         iterator begin() const { return _Base::begin(); }
//         iterator end() { return _Base::end(); }
//         iterator end() const { return _Base::end(); }
//         size_type size() const { return _Base::size(); }
//         size_type row_size() const { return _row_size; }
//         size_type col_size() const { return _col_size; }
//         inline __attribute__((__always_inline__))
//         reference operator()(size_type row_idx, size_type col_idx) noexcept
//         { return this->_M_impl._M_start[row_idx * _col_size + col_idx]; }
//         inline __attribute__((__always_inline__))
//         const_reference operator()(size_type row_idx, size_type col_idx) const noexcept
//         { return this->_M_impl._M_start[row_idx * _col_size + col_idx]; }
//         inline __attribute__((__always_inline__))
//         reference operator()(const std::pair<int, int>& coord) noexcept
//         { return this->_M_impl._M_start[coord.first * _col_size + coord.second]; }
//         inline __attribute__((__always_inline__))
//         const_reference operator()(const std::pair<int, int>& coord) const noexcept
//         { return this->_M_impl._M_start[coord.first * _col_size + coord.second]; }
//         _Self& operator=(const std::initializer_list<value_type>& init_list) { this->assign(init_list); return *this; }
//         _Self& set_value(const value_type& value) { this->assign(this->size(), value_type{}); return *this; }
//         friend std::ostream& operator<<(std::ostream& os, const _Self& matrix) {
//             for (int i = 0, n = matrix.row_size(); i < n; ++i) {
//                 for (int j = 0, m = matrix.col_size(); j < m; ++j)
//                 { os << matrix(i, j) << ' '; } os << std::endl;
//             } return os;
//         }
//         friend std::istream& operator>>(std::istream& is, _Self& matrix) {
//             for (int i = 0, n = matrix.row_size(); i < n; ++i) {
//                 for (int j = 0, m = matrix.col_size(); j < m; ++j) { is >> matrix(i, j); }
//             } return is;
//         }
//     private:
//         size_type _row_size, _col_size;
//     };
// }