#ifndef UTILITY_H
#define UTILITY_H
#include <utility>

namespace lcf {
    struct coord : std::pair<int, int> {
    #define x first
    #define y second
        coord() = default;
        template<typename Int>
        constexpr coord(Int&& first, Int&& second)
        : std::pair<int, int>(std::forward<Int>(first), std::forward<Int>(second)) { }
        friend coord operator + (const coord& lhs, const coord& rhs)
        { return coord(lhs.first + rhs.first, lhs.second + rhs.second); }
        friend coord& operator += (coord& lhs, const coord& rhs) {
            lhs.first += rhs.first; lhs.second += rhs.second;
            return lhs;
        }
    };
    template<typename Int>
    constexpr coord make_coord(Int&& first, Int&& second) 
    { return coord{std::forward<Int>(first), std::forward<Int>(second)}; }
}

namespace lcf {
    // Python-Like enumerate()
    template <typename IterableType, typename Iterator = typename std::decay_t<IterableType>::iterator>
    constexpr auto enumerate(IterableType && iterable) {
        struct iterator {
            bool operator != (const iterator & other) const { return iter != other.iter; }
            bool operator != (const Iterator& other) const { return iter != other; }
            void operator ++ () { ++index; ++iter; }
            auto operator * () const { return std::tie(index, *iter); }
            size_t index;
            Iterator iter;
        };
        struct iterable_wrapper {
            auto begin() {return iterator{0, std::begin(iterable)};}
            auto end() {return std::end(iterable);}
            IterableType iterable;
        };
        return iterable_wrapper{ std::forward<IterableType>(iterable) };  
    }
}

#endif