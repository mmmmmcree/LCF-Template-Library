#ifndef STRING_ALGORITHM_H
#define STRING_ALGORITHM_H
#include <string>
#include <vector>

namespace kmp {
    template<typename RandomAccessIterator>
    std::vector<int> get_lps_table(RandomAccessIterator first, RandomAccessIterator last) {
        size_t size = std::distance(first, last);
        std::vector<int> lps_table(size + 1, -1);
        for (size_t idx = 0, len = -1; idx < size; ++idx) {
            while (len != -1 and first[idx] != first[len]) { len = lps_table[len]; }
            lps_table[idx + 1] = ++len;
        }
        return lps_table;
    }

    template<typename FwdIterator, typename RAIterator, typename Functor>
    void match(FwdIterator first, FwdIterator last, RAIterator pattern_first, RAIterator pattern_last, Functor functor) {
        auto next = kmp::get_lps_table(pattern_first, pattern_last);
        size_t pattern_size = std::distance(pattern_first, pattern_last);
        for (size_t idx = 0, pattern_idx = 0; first != last; ++idx, ++first) {
            while (pattern_idx != 0 and pattern_first[pattern_idx] != *first) { pattern_idx = next[pattern_idx]; }
            if (pattern_first[pattern_idx] != *first) { continue; }
            if (++pattern_idx == pattern_size) {
                functor(idx);
                pattern_idx = next[pattern_idx];
            }
        }
    }

    template<typename RandomAccessIterator>
    std::vector<int> get_lcp_table(RandomAccessIterator first, RandomAccessIterator last) {
        //* box_range: [left, right)
        if (first == last) { return std::vector<int>(); }
        size_t size = std::distance(first, last);
        std::vector<int> lcp_table(size, 0); lcp_table[0] = size;
        for (size_t idx = 1, left = idx - 1, right = idx; idx < size; ++idx) {
            if (right > idx) { lcp_table[idx] = std::min(static_cast<size_t>(lcp_table[idx - left]), right - idx); }
            size_t right_idx = idx + lcp_table[idx];
            while (right_idx < size and first[right_idx] == first[lcp_table[idx]]) { ++right_idx; ++lcp_table[idx]; }
            if (right_idx > right) { left = idx; right = right_idx; }
        }
        return lcp_table;
    }

    template<typename TextIterator, typename PatternIterator>
    std::vector<int> get_lcp_table(TextIterator first, TextIterator last, PatternIterator pattern_first, PatternIterator pattern_last) {
        //* box_range: [left, right)
        if (first == last) { return std::vector<int>(); }
        size_t size = std::distance(first, last), pattern_size = std::distance(pattern_first, pattern_last);
        std::vector<int> pattern_lcp_table = get_lcp_table(pattern_first, pattern_last), lcp_table(size, 0);
        for (size_t idx = 0, left = idx - 1, right = idx; idx < size; ++idx) {
            if (right > idx) { lcp_table[idx] = std::min(static_cast<size_t>(pattern_lcp_table[idx - left]), right - idx); }
            size_t right_idx = idx + lcp_table[idx];
            while (right_idx < size and lcp_table[idx] < pattern_size and first[right_idx] == pattern_first[lcp_table[idx]])
            { ++right_idx; ++lcp_table[idx]; }
            if (right_idx > right) { left = idx; right = right_idx; }
        }
        return lcp_table;
    }
}

namespace lcf{
    // manacher, 返回最长回文串中心下标(偶数为靠右的那个)和长度
    std::pair<size_t, size_t> manacher(std::string_view strv) {
        size_t end_index = (strv.size() + 1) * 2;
        std::string str(end_index + 1, '#');
        for (size_t i = 0, j = 2; j != end_index; ++i, j += 2) {str[j] = strv[i];}
        str.front() = '@'; str.back() = '$';
        size_t central_index = 1, radius = 1;
        std::vector<size_t> dp(end_index + 1, 1);
        for (size_t i = 1; i != end_index; ++i) {
            if (i < central_index + radius)
            {dp[i] = std::min(dp[2 * central_index - i], central_index + radius - i);}
            size_t left = i - dp[i], right = i + dp[i];
            while (str[left--] == str[right++]) {++dp[i];}
            if (dp[i] > radius) {
                central_index = i;
                radius = dp[i];
            }
        }
        return std::make_pair((central_index - 1) / 2, radius - 1);
    }
}
#endif