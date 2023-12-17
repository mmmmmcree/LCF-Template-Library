#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <vector>

namespace lcf {
    class Timer {
    public:
        Timer() = default;
        ~Timer();
        static void start();
        static void end();
        template<typename Functor, typename... Args>
        void get_runtime(Functor func, Args&&... args) {
            auto start_point = std::chrono::steady_clock::now();
            func(std::forward<Args>(args)...);
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_point);
            _runtime.emplace_back(duration.count());
        }
    private:
        std::vector<std::chrono::_V2::system_clock::rep> _runtime;
        static std::chrono::_V2::system_clock::time_point _start_point;
    };

}

#endif