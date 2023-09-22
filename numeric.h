#ifndef NUMERIC_H
#define NUMERIC_H
#include <cmath>
#include <string>

namespace lcf {
    template<typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, std::string>
    convert_to_base(Integer num, size_t base) {
        std::string result;
        for (; num; num /= base) {
            if (Integer remainder = num % base; remainder > 9) { result.push_back(remainder + 87); }
            else { result.push_back(remainder + '0'); }
        }
        return std::string(result.rbegin(), result.rend());
    }

    template<size_t base, typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, std::string>
    convert_to_base(Integer num) {
        std::string result;
        for (; num; num /= base) {
            if constexpr (base > 9) {
                if (Integer remainder = num % base; remainder > 9) { result.push_back(remainder + 87); }
                else { result.push_back(remainder + '0'); }
            } else { result.push_back(num % base + '0'); }
        }
        return std::string(result.rbegin(), result.rend());
    }
}

namespace lcf {
    template<typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, std::pair<Integer, Integer>>
    extended_gcd(Integer a, Integer b) { // ax + by = gcd(a, b) * n
        if (b == 0) { return std::make_pair<Integer, Integer>(1, 0); }
        auto [x, y] = extended_gcd(b, a % b);
        return std::make_pair(y, x - a / b * y);
    }
}

namespace lcf {
    template<typename Integer, typename... RestTypes>
    std::enable_if_t<std::is_integral_v<Integer> and std::conjunction_v<std::is_same<Integer, RestTypes>...>, Integer>
    gcd(Integer first, Integer second, RestTypes... rest) {
        Integer result = first;
        if constexpr(sizeof...(rest) > 0) { result = gcd(result, rest...); }
        return std::gcd(result, second);
    }
}

namespace lcf {
    // 二维矩阵差分
    template<typename MatrixType>
    void matrix_adjacent_difference(MatrixType& matrix, size_t x1, size_t y1, size_t x2, size_t y2);

    // 二维矩阵前缀和
    template<typename MatrixType>
    void matrix_partial_sum(MatrixType& matrix, size_t x1, size_t y1, size_t x2, size_t y2);
}

template<typename MatrixType>
void lcf::matrix_adjacent_difference(MatrixType& matrix, size_t x1, size_t y1, size_t x2, size_t y2) {
    for (size_t i = x1; i <= x2; ++i) {
        for (size_t j = y2; j > y1; --j) {
            matrix[i][j] -= matrix[i][j - 1];
        }
    }
    for (size_t j = y1; j <= y2; ++j) {
        for (size_t i = x2; i > x1; --i) {
            matrix[i][j] -= matrix[i - 1][j];
        }
    }
}

template<typename MatrixType>
void lcf::matrix_partial_sum(MatrixType& matrix, size_t x1, size_t y1, size_t x2, size_t y2) {
    for (size_t i = x1; i <= x2; ++i) {
        for (size_t j = y1 + 1; j <= y2; ++j) {
            matrix[i][j] += matrix[i][j - 1];
        }
    }
    for (size_t j = y1; j <= y2; ++j) {
        for (size_t i = x1 + 1; i <= x2; ++i) {
            matrix[i][j] += matrix[i - 1][j];
        }
    }
}

namespace lcf{
    bool is_prime(long long n) {
        if (n <= 1) {return false;}
        for (long long i = 2, j = sqrt(n); i <= j; ++i) {
            if (n % i == 0) {return false;}
        }
        return true;
    }
}

#endif