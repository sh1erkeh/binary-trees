#pragma once

#include <chrono>
#include <iostream>
#include <variant>

class Timer {
    using nothing = std::monostate;
    using time_type = std::chrono::time_point<std::chrono::steady_clock>;

    std::variant<nothing, time_type> state;

public:
    Timer() : state(nothing{}) {}

    void start() {
        if (std::holds_alternative<time_type>(state)) {
            throw std::runtime_error("Timer already started");
        }
        state = std::chrono::steady_clock::now();
    }

    auto get_elapsed() {
        if (std::holds_alternative<nothing>(state)) {
            throw std::runtime_error("Timer not running");
        }

        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - std::get<time_type>(state)
        ).count();

        state = nothing{};
        return elapsed_time;
    }
};
