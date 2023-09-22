#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <functional>

namespace lcf {
    namespace BIT {
        template<typename RAIterator, typename Method = std::plus<typename RAIterator::value_type>>
        void make_tree(RAIterator first, RAIterator last, Method method = Method()) {
            size_t index = 1, end_index = std::distance(first, last);
            for (;index <= end_index; ++index) {
                size_t target_index = index + (index & -index);
                if (target_index <= end_index)
                {*(first + target_index - 1) = method(*(first + target_index - 1), *(first + index - 1));}
            }
        }
        template<typename RAIterator, typename Method = std::minus<typename RAIterator::value_type>>
        void break_tree(RAIterator first, RAIterator last, Method method = Method()) {
            size_t index = std::distance(first, last), end_index = index;
            for (;index > 0; --index) {
                size_t target_index = index + (index & -index);
                if (target_index <= end_index)
                {*(first + target_index - 1) = method(*(first + target_index - 1), *(first + index - 1));}
            }
        }
        template<typename RAIterator, typename Method = std::plus<typename RAIterator::value_type>>
        typename RAIterator::value_type query_tree(RAIterator first, RAIterator iter, Method method = Method()) {
            size_t index = std::distance(first, iter);
            typename RAIterator::value_type result = 0;
            for (; index > 0; index -= index & -index)
            {result = method(result, *(first + index - 1));}
            return result;
        }
        template<typename RAIterator, typename Method = std::plus<typename RAIterator::value_type>>
        void update_tree(RAIterator first, RAIterator iter, RAIterator last, typename RAIterator::value_type value, Method method = Method()) {
            size_t index = std::distance(first, iter) + 1;
            size_t end_index = std::distance(first, last);
            for (; index <= end_index; index += index & -index) {
                *(first + index - 1) = method(*(first + index - 1), value);
            }
        }
    }
}

namespace lcf {
    template<typename FirstType, typename SecondType, typename... RestTypes,
    typename ResultType = decltype(std::declval<FirstType>() + std::declval<SecondType>())>
    std::enable_if_t<std::is_arithmetic_v<ResultType> && std::conjunction_v<std::is_convertible<ResultType, RestTypes>...>, ResultType>
    max(FirstType first, SecondType second, RestTypes... rest) {
        ResultType result = first;
        if constexpr(sizeof...(rest) > 0) {result = max(result, rest...);}
        return result < second ? second : result;
    }
    template<typename FirstType, typename SecondType, typename... RestTypes,
    typename ResultType = decltype(std::declval<FirstType>() + std::declval<SecondType>())>
    std::enable_if_t<std::is_arithmetic_v<ResultType> && std::conjunction_v<std::is_convertible<ResultType, RestTypes>...>, ResultType>
    min(FirstType first, SecondType second, RestTypes... rest) {
        ResultType result = first;
        if constexpr(sizeof...(rest) > 0) {result = min(result, rest...);}
        return second < result ? second : result;
    }
}
#endif