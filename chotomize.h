#ifndef CHOTOMIZE_H
#define CHOTOMIZE_H
#include <type_traits>
#include <utility>

namespace lcf {
    // 整形二分，认为区间[left, right]中靠左部分check为true, 靠右部分check为false, 返回check为true的最大值
    template<typename IntegerType, typename CheckFunctor>
    std::enable_if_t<std::is_integral_v<IntegerType>, IntegerType>
    integer_dichotomize(IntegerType left, IntegerType right, CheckFunctor check);

    // 浮点型二分
    template<typename FloatType, typename CheckFunctor>
    std::enable_if_t<std::is_floating_point_v<FloatType>, FloatType>
    floating_point_dichotomize (FloatType left, FloatType right, const double precision, CheckFunctor check);

    // 整形三分
    template<typename IntegerType, typename CheckFunctor>
    std::enable_if_t<std::is_integral_v<IntegerType>, std::pair<IntegerType, IntegerType>>
    integer_trichotomize (IntegerType left, IntegerType right, CheckFunctor check);

    // 浮点型三分
    template<typename FloatType, typename CheckFunctor>
    std::enable_if_t<std::is_floating_point_v<FloatType>, FloatType>
    floating_point_trichotomize (FloatType left, FloatType right, const double precision, CheckFunctor check);
}

template<typename IntegerType, typename CheckFunctor>
std::enable_if_t<std::is_integral_v<IntegerType>, IntegerType>
lcf::integer_dichotomize(IntegerType left, IntegerType right, CheckFunctor check) {
    while (left < right) {
        IntegerType mid = left + (right - left + 1) / 2;
        if (check(mid)) {left = mid;}
        else {right = mid - 1;}
    }
    return left;
}

template<typename FloatType, typename CheckFunctor>
std::enable_if_t<std::is_floating_point_v<FloatType>, FloatType>
lcf::floating_point_dichotomize (FloatType left, FloatType right, const double precision, CheckFunctor check) {
    while (right - left > precision) {
        FloatType mid = left + (right - left) / 2;
        if (check(mid)) {left = mid;}
        else {right = mid;}
    }
    return left;
}

template<typename IntegerType, typename CheckFunctor>
std::enable_if_t<std::is_integral_v<IntegerType>, std::pair<IntegerType, IntegerType>>
lcf::integer_trichotomize (IntegerType left, IntegerType right, CheckFunctor check) {
    while (right - left >= 3) {
        IntegerType gap = (right - left) / 3;
        IntegerType mid_1 = left + gap, mid_2 = right - gap;
        if (check(mid_1, mid_2)) {left = mid_1;}
        else {right = mid_2;}
    }
    return std::make_pair(left, right);
}

template<typename FloatType, typename CheckFunctor>
std::enable_if_t<std::is_floating_point_v<FloatType>, FloatType>
lcf::floating_point_trichotomize (FloatType left, FloatType right, const double precision, CheckFunctor check) {
    while (right - left > precision) {
        FloatType gap = (right - left) / 3;
        FloatType mid_1 = left + gap, mid_2 = right - gap;
        if (check(mid_1, mid_2)) {left = mid_1;}
        else {right = mid_2;}
    }
    return left;
}

#endif
