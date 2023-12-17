#ifndef HEAP_H
#define HEAP_H
#include <ext/pb_ds/priority_queue.hpp>

namespace lcf {
    template <typename T, typename CmpFunctor>
    using std_binary_heap = std::priority_queue<T, std::vector<T>, CmpFunctor>;
    template <typename T, typename CmpFunctor>
    using pairing_heap = __gnu_pbds::priority_queue<T, CmpFunctor, __gnu_pbds::pairing_heap_tag>;
    template <typename T, typename CmpFunctor>
    using binary_heap = __gnu_pbds::priority_queue<T, CmpFunctor, __gnu_pbds::binary_heap_tag>;
    template <typename T, typename CmpFunctor>
    using fibonacci_heap = __gnu_pbds::priority_queue<T, CmpFunctor, __gnu_pbds::thin_heap_tag>;
    template <typename T, typename CmpFunctor>
    using binomial_heap = __gnu_pbds::priority_queue<T, CmpFunctor, __gnu_pbds::binomial_heap_tag>;
    template <typename T, typename CmpFunctor>
    using mergeable_binomial_heap = __gnu_pbds::priority_queue<T, CmpFunctor, __gnu_pbds::rc_binomial_heap_tag>;
}

#endif