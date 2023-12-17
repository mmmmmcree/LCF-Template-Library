#ifndef STACK_H
#define STACK_H
#include <concepts>
#include <array>

namespace lcf {
    template <typename T, std::size_t MAXSIZE>
    requires std::is_trivially_destructible_v<T>
    class static_stack {
        using _Sequence = std::array<T, MAXSIZE>;
    public:
        using size_type = typename _Sequence::size_type;
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        static_stack() : _top(-1) { }
        bool empty() const { return _top == -1; }
        bool full() const { return _top == c.size() - 1; }
        void push(const_reference val) { if (not full()) { c[++_top] = val; } }
        void pop() { if (not empty()) { --_top; } }
        reference top() { return c[_top]; }
        const_reference top() const { return c[_top]; }
    private:
        _Sequence c;
        size_type _top;
    };
}

#endif