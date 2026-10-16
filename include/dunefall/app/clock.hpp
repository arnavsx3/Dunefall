#pragma once

#include <algorithm>
#include <chrono>

namespace dunefall {

class FixedStep {
  public:
    using Duration = std::chrono::nanoseconds;
    static constexpr Duration interval{1'000'000'000 / 30};
    static constexpr int max_catch_up = 3;

    int advance(Duration elapsed, bool paused) noexcept {
        if (paused) {
            accumulated_ = Duration::zero();
            return 0;
        }
        const auto limit = interval * max_catch_up;
        accumulated_ = std::min(accumulated_ + std::clamp(elapsed, Duration::zero(), limit), limit);
        const auto steps = static_cast<int>(accumulated_ / interval);
        accumulated_ -= interval * steps;
        return steps;
    }

  private:
    Duration accumulated_{};
};

} // namespace dunefall
