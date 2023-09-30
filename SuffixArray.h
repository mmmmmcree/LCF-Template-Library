#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H
#include <vector>
#include <numeric>

namespace lcf {
    template<typename StringType>
    class SuffixArray {
    public:
        using string_type = StringType;
        using array_type = std::vector<int>;
        using value_type = typename array_type::value_type;
        using reference = typename array_type::reference;
        using const_reference = typename array_type::const_reference;
        using iterator = typename array_type::iterator;
        SuffixArray(string_type&& str, size_t set_size = 128) : _str(std::forward<string_type>(str)), _suffix_array(std::size(_str))
        { _build_suffix_array(std::begin(_str), std::end(_str), set_size); }
        // SuffixArray(const string_type& str, size_t set_size = 128) : _str(str), _suffix_array(std::size(_str))
        // { _build_suffix_array(std::begin(_str), std::end(_str), set_size); }
        std::string_view str_view() const { return _str; }
        array_type get_height() const {
            size_t size = std::size(_str);
            array_type height(size), rank(size);
            for (size_t i = 0; i < size; ++i) { rank[_suffix_array[i]] = i; }
            for (size_t idx = 0, box_size = 0; idx < size; ++idx) {
                if (rank[idx] == 0) { continue; }
                size_t  pre_idx = _suffix_array[rank[idx] - 1];
                if (box_size > 0) { --box_size; }
                while (std::max(idx, pre_idx) + box_size < size and _str[idx + box_size] == _str[pre_idx + box_size]) { ++box_size; }
                height[rank[idx]] = box_size;
            }
            return height;
        }
        iterator begin() { return _suffix_array.begin(); }
        iterator end() { return _suffix_array.end(); }
        inline __attribute__((always_inline))
        reference operator[](size_t index) { return _suffix_array[index]; }
        inline __attribute__((always_inline))
        const_reference operator[](size_t index) const { return _suffix_array[index]; }
    private:
        template<typename ForwardIterator>
        void _build_suffix_array(ForwardIterator first, ForwardIterator last, size_t set_size) {
            if (first == last) { return; }
            size_t size = std::distance(first, last);
            std::vector<int> buckets(std::max(size, set_size), 0), hash(first, last), key_str_idxes(size);
            auto buckets_begin = buckets.begin();
            for (auto hash_idx : hash) { ++buckets[hash_idx]; }
            std::partial_sum(buckets_begin, buckets_begin + set_size, buckets_begin);
            for (size_t rank = size - 1; rank != -1; --rank) { _suffix_array[--buckets[hash[rank]]] = rank; }
            for (size_t gap = 1; gap < size; gap <<= 1) {
                size_t rank = 0;
                for (size_t escaped_idx = size - gap; escaped_idx < size; ++escaped_idx) { key_str_idxes[rank++] = escaped_idx; }
                for (auto key_char_idx : _suffix_array) { if (key_char_idx >= gap) { key_str_idxes[rank++] = key_char_idx - gap; } }
                std::fill_n(buckets_begin, set_size, 0);
                for (auto hash_idx : hash) { ++buckets[hash_idx]; }
                std::partial_sum(buckets_begin, buckets_begin + set_size, buckets_begin);
                for (--rank; rank != -1; --rank) { _suffix_array[--buckets[hash[key_str_idxes[rank]]]] = key_str_idxes[rank]; }
                std::swap(key_str_idxes, hash);
                hash[_suffix_array[0]] = set_size = 0;
                for (size_t i = 1; i < size; ++i) {
                    auto lhs_key_str = key_str_idxes[_suffix_array[i - 1]], rhs_key_str = key_str_idxes[_suffix_array[i]];
                    auto lhs_key_char = key_str_idxes[_suffix_array[i - 1] + gap], rhs_key_char = key_str_idxes[_suffix_array[i] + gap];
                    if (lhs_key_str == rhs_key_str and lhs_key_char == rhs_key_char) { hash[_suffix_array[i]] = set_size; }
                    else { hash[_suffix_array[i]] = ++set_size; }
                }
                if (++set_size == size) { break; }
            }
        }
    private:
        string_type _str;
        array_type _suffix_array;
    };
}

#endif