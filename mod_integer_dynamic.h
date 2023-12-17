#ifndef MOD_INTEGER_DYNAMIC_H
#define MOD_INTEGER_DYNAMIC_H
#include <utility>

namespace lcf {
    template<typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, std::pair<Integer, Integer>>
    extended_gcd(Integer a, Integer b) { // ax + by = gcd(a, b) * n
        if (b == 0) { return std::make_pair<Integer, Integer>(1, 0); }
        auto [x, y] = extended_gcd(b, a % b);
        return std::make_pair(y, x - a / b * y);
    }

    template<typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, Integer>
    mod_positive(Integer value, Integer modulus)
    { return (value % modulus + modulus) % modulus; }

    template<typename Integer>
    class mod_integer {
        using _Self = mod_integer<Integer>;
    public:
        using value_type = Integer;
        static void set_moder(const size_t _MODULUS) { MODULUS = _MODULUS; }
        static _Self A(size_t n, size_t r);
        static _Self C(size_t n, size_t r);
        static _Self Lucas(size_t n, size_t r);
        constexpr mod_integer(value_type value = 0) : _value(value) { }
        mod_integer(const _Self&) = default;
        value_type positive_value() const { return (_value % MODULUS + MODULUS) % MODULUS; }
        value_type actual_value() const { return _value; }
        //! bool invertible() { return std::gcd(_value, MODULUS) == 1; }
        value_type mod_inverse(value_type moder) const { return lcf::extended_gcd(_value, moder).first; }
        friend _Self operator+(const _Self& lhs, const _Self& rhs)
        { return _Self((lhs._value + rhs._value) % MODULUS); }
        friend _Self& operator+=(_Self& lhs, const _Self& rhs)
        { (lhs._value += rhs._value) %= MODULUS; return lhs; }
        friend _Self operator-(const _Self& lhs, const _Self& rhs)
        { return _Self((lhs._value - rhs._value + MODULUS) % MODULUS); }
        friend _Self& operator-=(_Self& lhs, const _Self& rhs)
        { ((lhs._value -= rhs._value) += MODULUS) %= MODULUS; return lhs; }
        friend _Self operator*(const _Self& lhs, const _Self& rhs)
        { return _Self(lhs._value * rhs._value % MODULUS); }
        friend _Self& operator*=(_Self& lhs, const _Self& rhs)
        { (lhs._value *= rhs._value) %= MODULUS; return lhs; }
        friend _Self operator^(_Self base, size_t exponent) {
            _Self power(1);
            for (; exponent > 0; exponent >>= 1, base *= base) {
                if (exponent & 1) { power *= base; }
            }
            return power;
        }
        friend _Self& operator^=(_Self& power, size_t exponent) {
            if (exponent-- == 0) { power._value = 1; return power; }
            for (_Self base = power; exponent > 0; exponent >>= 1, base *= base) {
                if (exponent & 1) { power *= base; }
            }
            return power;
        }
        friend _Self operator/(const _Self& lhs, const _Self& rhs)
        { return _Self(lhs._value * rhs.mod_inverse(MODULUS) % MODULUS); }
        friend _Self& operator/=(_Self& lhs, const _Self& rhs)
        { (lhs._value *= rhs.mod_inverse(MODULUS)) %= MODULUS; return lhs; }
        friend bool operator<(const _Self& lhs, const _Self& rhs) { return lhs._value < rhs._value; }
        friend bool operator>(const _Self& lhs, const _Self& rhs) { return lhs._value > rhs._value; }
        friend bool operator==(const _Self& lhs, const _Self& rhs) { return lhs._value == rhs._value; }
        friend bool operator!=(const _Self& lhs, const _Self& rhs) { return lhs._value != rhs._value; }
        friend bool operator<=(const _Self& lhs, const _Self& rhs)  { return lhs._value <= rhs._value; }
        friend bool operator>=(const _Self& lhs, const _Self& rhs)  { return lhs._value >= rhs._value; }
        friend std::ostream& operator<<(std::ostream& os, const _Self& self) { return os << self.positive_value(); }
        friend std::istream& operator>>(std::istream& is, _Self& self) { return is >> self._value; }
        friend _Self slow_mul(_Self lhs, _Self rhs) { 
            _Self result(0);
            for (; rhs._value > 0; rhs._value >>= 1) {
                if (rhs._value & 1) { result += lhs; }
                lhs += lhs;
            }
            return result;
        }
    private:
        std::enable_if_t<std::is_integral_v<value_type>, value_type> _value;
    public:
        inline static value_type MODULUS = 1e9 + 7;
    };

    template<typename Integer>
    mod_integer<Integer> mod_integer<Integer>::A(size_t n, size_t r) {
        if (r > n) { return 0; }
        r = std::min(r, n - r);
        if (r == 0) { return 1; }
        mod_integer<Integer> num(1);
        for (size_t i = n; i > n - r; --i) { num *= i; }
        return num;
    }

    template<typename Integer>
    mod_integer<Integer> mod_integer<Integer>::C(size_t n, size_t r) {
        if (r > n) { return 0; }
        r = std::min(r, n - r);
        if (r == 0) { return 1; }
        mod_integer<Integer> num(1), den(1);
        for (size_t i = n; i > n - r; --i) { num *= i; }
        for (size_t i = r; i > 0; --i) { den *= i; }
        return num / den;
    }

    template<typename Integer>
    mod_integer<Integer> mod_integer<Integer>::Lucas(size_t n, size_t r) {
        if (r == 0) { return 1; }
        return Lucas(n / MODULUS, r / MODULUS) * C(n % MODULUS, r % MODULUS);
    }
}

namespace lcf {
    template<typename Integer>
    std::enable_if_t<std::is_integral_v<Integer>, Integer>
    phi(Integer num) {
        Integer result = num;
        for (size_t i = 2, n = sqrt(num); i <= n; ++i) {
            if (num % i == 0) { result = result / i * (i - 1); }
            while (num % i == 0) { num /= i; }
        }
        if (num > 1) { result = result / num *(num - 1); }
        return result;
    }
}

#include <vector>

namespace lcf {
    std::vector<size_t> euler_sieve(size_t n) {
        enum Bool : bool {FALSE, TRUE};
        std::vector<Bool> vis(n + 1, FALSE);
        std::vector<size_t> primes;
        for (size_t i = 2; i <= n; ++i) {
            if (vis[i] == FALSE) { primes.push_back(i); }
            for (size_t j = 0, next_idx; (next_idx = primes[j] * i) <= n; ++j) {
                vis[next_idx] = TRUE;
                if (i % primes[j] == 0) { break; }
            }
        }
        return primes;
    } 
}

namespace lcf {
    std::vector<int> get_phi(size_t n) {
        if (n == 0) { return std::vector<int>{0}; }
        enum Bool : bool {FALSE, TRUE};
        std::vector<Bool> vis(n + 1, FALSE);
        std::vector<int> primes, phi(n + 1); phi[1] = 1;
        for (size_t i = 2; i <= n; ++i) {
            if (vis[i] == FALSE) { primes.emplace_back(i); phi[i] = i - 1; }
            for (size_t j = 0, next_idx; (next_idx = primes[j] * i) <= n; ++j) {
                vis[next_idx] = TRUE;
                if (i % primes[j] == 0) { phi[next_idx] = primes[j] * phi[i]; break; }
                phi[next_idx] = phi[primes[j]] * phi[i];
            }
        }
        return phi;
    } 
}

namespace lcf {
    std::vector<int> get_divisor_nums(size_t n) {
        if (n == 0) { return std::vector<int>{0}; }
        enum Bool : bool {FALSE, TRUE};
        std::vector<Bool> vis(n + 1, FALSE);
        std::vector<int> primes, min_exponents(n + 1, 1), divisor_nums(n + 1, 2); divisor_nums[1] = 1;
        for (size_t i = 2; i <= n; ++i) {
            if (vis[i] == FALSE) { primes.emplace_back(i); }
            for (size_t j = 0, next_idx; (next_idx = primes[j] * i) <= n; ++j) {
                vis[next_idx] = TRUE;
                if (i % primes[j] == 0) {
                    min_exponents[next_idx] = min_exponents[i] + 1;
                    divisor_nums[next_idx] = divisor_nums[i] / min_exponents[next_idx] * (min_exponents[next_idx] + 1);
                    break;
                }
                divisor_nums[next_idx] = divisor_nums[i] * 2;
            }
        }
        return divisor_nums;
    }
}


#endif