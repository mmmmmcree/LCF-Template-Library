#ifndef FIBONACCI_SEQUENCE_H
#define FIBONACCI_SEQUENCE_H
namespace lcf{
    template<size_t n, unsigned int MODER>
    struct _Fibonacci : _Fibonacci<n - 1, MODER>{
        static constexpr typename _Fibonacci<n - 1, MODER>::value_type
        value = (_Fibonacci<n - 1, MODER>::value + _Fibonacci<n - 2, MODER>::value) % MODER;
    };

    template<unsigned int MODER>
    struct _Fibonacci<1, MODER> {
        using value_type = unsigned long long;
        static constexpr value_type value = 1;
    };

    template<unsigned int MODER>
    struct _Fibonacci<0, MODER> {
        using value_type = unsigned long long;
        static constexpr value_type value = 0;
    };

    template<unsigned int MODER = 1000000007>
    struct Fibonacci{
        template<size_t n> using Generator = _Fibonacci<n, MODER>;
    };
}
#endif