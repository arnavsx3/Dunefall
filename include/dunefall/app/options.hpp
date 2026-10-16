#pragma once

#include <cstdint>
#include <ostream>
#include <span>
#include <string_view>

namespace dunefall {
struct Options {
    std::uint32_t seed = 42;
    int width = 64;
    int height = 24;
    int steps = 120;
    bool empty = false;
    bool headless = false;
    bool benchmark = false;
    bool help = false;
    bool version = false;
};

Options parse_options(std::span<const std::string_view> arguments);
int run_headless(const Options& options, std::ostream& output);
} // namespace dunefall
