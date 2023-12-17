#ifndef QUEUE_H
#define QUEUE_H
#include <array>

namespace lcf {
    template <typename T, std::size_t MAXSIZE>
    requires std::is_trivially_destructible_v<T>
    class circular_queue {
        using _Sequence = std::array<T, MAXSIZE + 1>;
    public:
        using size_type = typename _Sequence::size_type;
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        circular_queue() : head(0), tail(0) { }
        bool empty() const { return head == tail; }
        bool full() const { return (tail + 1) % c.size() == head; }
        void push(const_reference val) { if (not full()) { c[tail] = val; ++tail %= c.size(); } }
        void pop() { if (not empty()) { ++head %= c.size(); } }
        reference front() { return c[head]; }
        const_reference front() const { return c[head]; }
        reference back() { return c[tail]; }
        const_reference back() const { return c[tail]; }
    private:
        _Sequence c;
        size_type head, tail;
    };
}

#endif