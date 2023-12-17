#ifndef CHOTOMIZE_H
#define CHOTOMIZE_H
#include <concepts>

namespace lcf {
    // 整形二分，认为区间[left, right]中靠左部分check为true, 靠右部分check为false, 返回check为true的最大值
    template<std::integral Int, typename CheckFunctor>
    Int binary_search(Int left, Int right, CheckFunctor check) {
        while (left < right) {
            Int mid = left + (right - left + 1) / 2;
            if (check(mid)) {left = mid;}
            else {right = mid - 1;}
        }
        return left;
    }
    template<std::floating_point Float, typename CheckFunctor>
    Float binary_search(Float left, Float right, CheckFunctor check, const double eps = 1e-9) {
        while (right - left > eps) {
            Float mid = left + (right - left) / 2;
            if (check(mid)) {left = mid;}
            else {right = mid;}
        }
        return left;
    }
}

#endif
