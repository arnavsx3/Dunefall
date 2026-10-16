#include "dunefall/app/clock.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    try {
        using Clock = dunefall::FixedStep;
        Clock clock;
        const auto half = Clock::interval / 2;
        if (clock.advance(half, false) != 0 || clock.advance(Clock::interval - half, false) != 1) {
            throw std::runtime_error("Sub-frame time must accumulate into exactly one step");
        }
        if (clock.advance(half, false) != 0 ||
            clock.advance(std::chrono::seconds(10), false) != 3) {
            throw std::runtime_error(
                "Long stalls plus residual time must never exceed three steps");
        }
        if (clock.advance(half, false) != 0 || clock.advance(std::chrono::seconds(5), true) != 0 ||
            clock.advance(half, false) != 0) {
            throw std::runtime_error("Pause must discard accumulated time without catch-up");
        }
        if (clock.advance(Clock::Duration{-1}, true) != 0 ||
            clock.advance(Clock::Duration{-1}, false) != 0 ||
            clock.advance(Clock::interval, false) != 1) {
            throw std::runtime_error("Negative elapsed time must not rewind the clock");
        }
        std::cout
            << "Clock: fractional steps, bounded catch-up, pause, and negative duration passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
