#ifndef DISCRETIZE_H
#define DISCRETIZE_H
#include <vector>
#include <algorithm>

namespace lcf {
    template<typename T>
    class enumerate {
        template<typename RIterator> friend void unique_discretize(RIterator, RIterator, size_t);
        template<typename RIterator> friend void discretize(RIterator, RIterator, size_t);
    public:
        enumerate(const T& _value) : value(_value), index(static_index++) { }
        friend bool operator<(const enumerate& lhs, const enumerate& rhs) { return lhs.value < rhs.value; }
        friend bool operator>(const enumerate& lhs, const enumerate& rhs) { return lhs.value > rhs.value; }
    private:
        T value;
        size_t index;
        inline static size_t static_index;
        static void init() { static_index = 0; }
    };

    template <typename RandomAccessIterator>
    void unique_discretize(RandomAccessIterator first, RandomAccessIterator last, size_t init_index = 0) {
        using value_type = typename RandomAccessIterator::value_type;
        enumerate<value_type>::init();
        std::vector<enumerate<value_type>> enumerate_data(first, last);
        std::sort(enumerate_data.begin(), enumerate_data.end());
        for (const auto& item : enumerate_data) { first[item.index] = init_index++; }
    }

    template <typename RandomAccessIterator>
    void discretize(RandomAccessIterator first, RandomAccessIterator last, size_t init_index = 0) {
        using value_type = typename RandomAccessIterator::value_type;
        enumerate<value_type>::init();
        std::vector<enumerate<value_type>> enumerate_data(first, last);
        auto begin = enumerate_data.begin(), end = enumerate_data.end();
        std::sort(begin, end);
        for (; begin != end; ++init_index) {
            for (auto value = begin->value; begin != end && begin->value == value; ++begin)
            { first[begin->index] = init_index; }
        }
    }
}

#endif