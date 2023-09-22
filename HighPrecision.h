#ifndef HIGHPRECISION_H
#define HIGHPRECISION_H
#include <string>
namespace lcf {
    class HighPrecision : public std::string {
    public:
        HighPrecision(const std::string& str) : std::string(str) {}
        HighPrecision(const char* str) : std::string(str) {}
        HighPrecision(std::string&& str) : std::string(str) {}
        HighPrecision(size_t n, char ch) : std::string(n, ch) {}
        HighPrecision() = default;
        HighPrecision(const HighPrecision&) = default;
        HighPrecision(HighPrecision&&) = default;
        HighPrecision(std::string::iterator begin, std::string::iterator end) : std::string(begin, end) {}
        HighPrecision(std::string::reverse_iterator rbegin, std::string::reverse_iterator rend) : std::string(rbegin, rend) {}
        HighPrecision& operator=(const HighPrecision&) = default;
        HighPrecision& operator=(HighPrecision&&) = default;
        friend bool operator<(const HighPrecision& lhs, const HighPrecision& rhs) {
            if (lhs.size() == rhs.size()) {
                for (size_t i = 0, n = lhs.size(); i < n; ++i) {return lhs[i] < rhs[i];}
            }
            return lhs.size() < rhs.size();
        }
        friend bool operator>(const HighPrecision& lhs, const HighPrecision& rhs) {
            if (lhs.size() == rhs.size()) {
                for (size_t i = 0, n = lhs.size(); i < n; ++i) {return lhs[i] > rhs[i];}
            }
            return lhs.size() > rhs.size();
        }
        friend bool operator<=(const HighPrecision& lhs, const HighPrecision& rhs) {return !(lhs > rhs);}
        friend bool operator>=(const HighPrecision& lhs, const HighPrecision& rhs) {return !(lhs < rhs);}
        friend bool operator==(const HighPrecision& lhs, const HighPrecision& rhs) {
            if (lhs.size() != rhs.size()) {return false;}
            for (size_t i = 0, n = lhs.size(); i < n; ++i) {
                if (lhs[i] != rhs[i]) {return false;}
            }
            return true;
        }
        friend HighPrecision operator+(const HighPrecision& lhs, const HighPrecision& rhs) {
            HighPrecision result;
            auto lhs_riter = lhs.rbegin(), lhs_rend = lhs.rend(), rhs_riter = rhs.rbegin(), rhs_rend = rhs.rend();
            short cursor = 0;
            for (; lhs_riter != lhs_rend && rhs_riter != rhs_rend; ++lhs_riter, ++rhs_riter) {
                result.push_back('0' + (*lhs_riter + *rhs_riter + cursor - 2 * '0') % 10);
                cursor = (*lhs_riter + *rhs_riter  + cursor - 2 * '0') / 10;
            }
            for (; lhs_riter != lhs_rend; ++lhs_riter) {
                result.push_back('0' + (*lhs_riter + cursor - '0') % 10);
                cursor = (*lhs_riter + cursor - '0') / 10;
            }
            for (; rhs_riter != rhs_rend; ++rhs_riter) {
                result.push_back('0' + (*rhs_riter + cursor - '0') % 10);
                cursor = (*rhs_riter + cursor - '0') / 10;
            }
            if (cursor) {result.push_back('0' + cursor);}
            return HighPrecision(result.rbegin(), result.rend());
        }
        HighPrecision& operator+=(const HighPrecision& rhs) {return *this = *this + rhs;}
        friend HighPrecision operator-(const HighPrecision& lhs, const HighPrecision& rhs) {
            if (lhs == rhs) {return "0";}
            bool is_negative = (lhs < rhs);
            const HighPrecision& _lhs = is_negative ? rhs : lhs, _rhs = is_negative ? lhs : rhs;
            HighPrecision result;
            auto lhs_riter = _lhs.rbegin(), lhs_rend = _lhs.rend(), rhs_riter = _rhs.rbegin(), rhs_rend = _rhs.rend();
            short cursor = 0;
            for (; lhs_riter != lhs_rend && rhs_riter != rhs_rend; ++lhs_riter, ++rhs_riter) {
                if (*lhs_riter - cursor < *rhs_riter) {
                    result.push_back('0' + (*lhs_riter - cursor + 10 - *rhs_riter) % 10);
                    cursor = 1;
                } else {
                    result.push_back('0' + (*lhs_riter - cursor - *rhs_riter) % 10);
                    cursor = 0;
                }
            }
            for (; lhs_riter != lhs_rend; ++lhs_riter) {
                if (*lhs_riter - cursor < '0') {
                    result.push_back('0' + (*lhs_riter - cursor + 10 - '0') % 10);
                    cursor = 1;
                } else {
                    result.push_back('0' + (*lhs_riter - cursor - '0') % 10);
                    cursor = 0;
                }
            }
            if (is_negative) {result.push_back('-');}
            return HighPrecision(result.rbegin(), result.rend());
        }
        HighPrecision& operator-=(const HighPrecision& rhs) {return *this = *this - rhs;}
        friend HighPrecision operator*(const HighPrecision& lhs, const HighPrecision& rhs) {
            size_t lhs_size = lhs.size(), rhs_size = rhs.size(), size = lhs_size + rhs_size;
            HighPrecision result(size, '0');
            for (size_t i = lhs_size - 1; i != -1; --i) {
                for (size_t j = rhs_size - 1; j != -1; --j) {
                    result[size - i - j - 1] += (result[size - i - j - 2] - '0' + (lhs[i] - '0') *(rhs[j] - '0')) / 10;
                    result[size - i - j - 2] = '0' + (result[size - i - j - 2] - '0' + (lhs[i] - '0') *(rhs[j] - '0')) % 10;
                }
            }
            while (result.size() > 1 && result.back() == '0') {result.pop_back();}
            return HighPrecision(result.rbegin(), result.rend());
        }
        HighPrecision& operator*=(const HighPrecision& rhs) {return *this = *this * rhs;}
    };
}

#endif