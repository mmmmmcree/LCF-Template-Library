#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <utility>
#include <array>
#include "fibonacci_sequence.h"
#include "geometric_sequence.h"

namespace lcf {
    template<template<size_t>typename ValueGenerator, size_t n, size_t... Indexes>
    std::array<typename ValueGenerator<0>::value_type, n> _get_sequence(const std::index_sequence<Indexes...>&) {
        return std::array<typename ValueGenerator<0>::value_type, n>{ValueGenerator<Indexes>::value...};
    }

    template<template<size_t>typename ValueGenerator, size_t n>
    std::array<typename ValueGenerator<0>::value_type, n> get_sequence() {
        return _get_sequence<ValueGenerator, n>(std::make_index_sequence<n>());
    }
}

#endif