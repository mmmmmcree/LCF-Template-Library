#ifndef FAST_POW_H
#define FAST_POW_H
#include <type_traits>
#include <corecrt.h>

namespace lcf {
    template<typename T, typename = std::void_t<decltype(std::declval<T&>() *= std::declval<T>())>>
    std::enable_if_t<std::is_trivially_copy_assignable_v<T>, T>
    fastpow(T base, size_t exponent) {
        T power(1);
        for (; exponent > 0; exponent >>= 1, base *= base) {
            if (exponent & 1) { power *= base; }
        }
        return power;
    }
}

namespace lcf {
    template<typename T, typename = std::void_t<decltype(std::declval<T>() * std::declval<T>())>>
    std::enable_if_t<!std::is_trivially_copy_assignable_v<T>, T&>
    fast_pow(T& base, size_t exponent) {
        if (exponent-- == 0) { return base; }
        for (T product(base); exponent > 0; exponent >>= 1, product = product * product)
        { if (exponent & 1) { base = base * product; } }
        return base;
    }
}

#endif