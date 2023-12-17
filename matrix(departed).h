#include <iterator>
#include <memory>
#include <cstring>
namespace lcf {
    // Matrix_base负责控制容器的内存布局(如果内存管理十分复杂, base类就显得很有必要), 具体的分配和回收工作交由Allocator完成
    template<typename Type, typename AllocatorType>
    class Matrix_base {
    protected:
        Matrix_base(size_t row, size_t col)
        {
            _impl.row_size = row; _impl.col_size = col;
            _impl.ptr = std::allocator_traits<AllocatorType>::allocate(_impl, row * col);
        }
        Matrix_base(const Matrix_base& other) : _impl(other._impl) {
            _impl.ptr = std::allocator_traits<AllocatorType>::allocate(_impl, _impl.row_size * _impl.col_size);
        }
        ~Matrix_base() {
            std::allocator_traits<AllocatorType>::deallocate(_impl, _impl.ptr, _impl.row_size * _impl.col_size);
            _impl.ptr = nullptr;
        }
        // Matrix_impl_data负责管理成员变量
        struct Matrix_impl_data {
            size_t row_size;
            size_t col_size;
            Type* ptr; // 考虑到矩阵的特性, 这里先不使用迭代器
        };
        // 通过继承加入Allocator, 不占用额外空间, 并负责与Allocator有关的衔接操作
        struct Matrix_impl : AllocatorType, Matrix_impl_data {};
    protected:    
        Matrix_impl _impl; // base中唯一成员变量, 负责与Matrix类交互
    };

    template<typename Type, typename AllocatorType = std::allocator<Type>>
    class matrix : protected Matrix_base<Type, AllocatorType> {
        using Base = Matrix_base<Type, AllocatorType>;
        using Base::_impl;
    public:
        using value_type = Type;
        using pointer = value_type*;
        using reference = value_type&;
        // 容器的迭代器理应有一份与内存布局相匹配的独立设计, 不过matrix类的内存布局十分简单, 用现成的即可
        using iterator = __gnu_cxx::__normal_iterator<pointer, matrix<value_type>>;
        using reverse_iterator = std::reverse_iterator<iterator>;
    public:
        // 元素的构造与析构由容器本身负责
        template<typename... Args>
        matrix(size_t row_size, size_t col_size, Args&&... args) : Base(row_size, col_size)
        { _init_construct(std::forward<Args>(args)...); }
        matrix(const matrix& other) : Base(other) { _copy_construct(other); }
        ~matrix() { _destruct(); }
        // 由以下这些函数从_impl中拿取必要的数据信息
        size_t row_size() const noexcept { return _impl.row_size; }
        size_t col_size() const noexcept { return _impl.col_size; }
        iterator begin() const noexcept { return iterator(_impl.ptr); }
        iterator end() const noexcept { return iterator(_impl.ptr + _impl.row_size * _impl.col_size); }
        // 与_impl的交互到此为止, 以后所有操作都由迭代器接管
        reverse_iterator rbegin() const noexcept { return reverse_iterator(this->end()); }
        reverse_iterator rend() const noexcept { return reverse_iterator(this->begin()); }
        matrix& operator=(const matrix& other) noexcept {
            if (this == std::addressof(other)) { return *this; }
            if (this->row_size() != other.row_size() || this->col_size() != other.col_size()) { return *this; }
            _destruct();
            _copy_construct(other);
            return *this;
        }
        inline __attribute__((__always_inline__))
        reference operator()(size_t row_index, size_t col_index) const noexcept
        { return _impl.ptr[row_index * _impl.col_size + col_index]; } 
        inline __attribute__((__always_inline__))
        reference operator()(const std::pair<size_t, size_t>& coordinate) const noexcept
        { return this->operator()(coordinate.first, coordinate.second); }
    private:
        template<typename... Args>
        void _init_construct(Args&&... args) noexcept {
            if constexpr (sizeof...(Args) == 0) {
                if constexpr (std::is_trivially_constructible_v<Type>) {
                    memset(this->begin().base(), 0, sizeof(Type) * this->row_size() * this->col_size());
                } else {
                    std::uninitialized_default_construct(this->begin(), this->end());
                }
            } else if constexpr (sizeof...(Args) == 1) {
                std::uninitialized_fill(this->begin(), this->end(), std::forward<Args>(args)...);
            } else {
                for (iterator iter = this->begin(), end = this->end(); iter != end; ++iter)
                { new(iter.base()) Type(std::forward<Args>(args)...); }
            }
        }
        void _copy_construct(const matrix& other) noexcept {
            if constexpr (std::is_trivially_constructible_v<Type>) {
                memcpy(this->begin().base(), other.begin().base(), sizeof(Type) * this->row_size() * this->col_size());
            } else { std::uninitialized_copy(other.begin(), other.end(), this->begin()); }
        }
        void _destruct() noexcept {
            if constexpr(!std::is_trivially_destructible_v<Type>)
            { std::destroy(this->begin(), this->end()); }
        }
    };
}