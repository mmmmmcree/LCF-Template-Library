#ifndef GEOMETRIC_SEQUENCE_H
#define GEOMETRIC_SEQUENCE_H
namespace lcf{
    template<long long num, long long den, size_t n>
    struct _Geometric : _Geometric<num, den, n - 1>{
        using value_type = typename _Geometric<num, den, n - 1>::value_type;
        static constexpr value_type value = static_cast<value_type>(num) / den * _Geometric<num, den, n - 1>::value;
    };

    template<long long num, long long den>
    struct _Geometric<num, den, 0> {
        using value_type = double;
        static constexpr value_type value = 1;
    };

    template<long long num, long long den>
    struct Geometric {
        template<size_t n> using Generator = _Geometric<num, den, n>;
    };

    template<unsigned int common_ratio, size_t n, unsigned int MODER>
    struct _UnsignedIntGeometric : _UnsignedIntGeometric<common_ratio, n - 1, MODER>{
        using value_type = typename _UnsignedIntGeometric<common_ratio, n - 1, MODER>::value_type;
        static constexpr value_type value = common_ratio * _UnsignedIntGeometric<common_ratio, n - 1, MODER>::value % MODER;
    };

    template<unsigned int common_ratio, unsigned int MODER>
    struct _UnsignedIntGeometric<common_ratio, 0, MODER> {
        using value_type = unsigned int;
        static constexpr value_type value = 1;
    };

    template<unsigned int common_ratio, unsigned int MODER = 1000000007>
    struct UnsignedIntGeometric {
        template<size_t n> using Generator = _UnsignedIntGeometric<common_ratio, n, MODER>;
    };
}
#endif