#include "Timer.h"
#include <iostream>
#include <format>

std::chrono::_V2::system_clock::time_point lcf::Timer::_start_point;

lcf::Timer::~Timer() {
    for (size_t index = 0; const auto& item : _runtime)
    { std::cout << std::format("The running time of functin[{}] is {} microseconds", index++, item) << std::endl; }
}

void lcf::Timer::start() {
    std::cout << "timer started..." << std::endl;
    _start_point = std::chrono::system_clock::now();
}

void lcf::Timer::end() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - _start_point);
    std::cout << std::format("timer ended... ({} microseconds)", duration.count()) << std::endl;
}