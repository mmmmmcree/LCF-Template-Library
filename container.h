#ifndef CONTAINER_H
#define CONTAINER_H
#include <cstring>
#include <type_traits>

namespace lcf {
    template<typename T, typename Iterator, typename SiztType = size_t>
    struct Container {
        using value_type = T;
        using iterator = Iterator;
        using size_type = SiztType;
    };
}

#endif