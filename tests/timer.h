#pragma once

#include <chrono>
#include <iostream>

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

    void stop() {
        if (std::holds_alternative<nothing>(state)) {
            throw std::runtime_error("Timer not started");
        }
        state = nothing{};
    }

    auto get_elapsed() const {
        if (std::holds_alternative<nothing>(state)) {
            throw std::runtime_error("Timer not running");
        }
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - std::get<time_type>(state)
        ).count();
    }
};
