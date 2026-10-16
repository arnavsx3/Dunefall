#include "dunefall/app/options.hpp"
#include "dunefall/simulation/world.hpp"

#include <charconv>
#include <stdexcept>
#include <string>

namespace dunefall {
namespace {
template <typename Number> Number number(std::string_view option, std::string_view value) {
    Number result{};
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || end != value.data() + value.size()) {
        throw std::invalid_argument("Invalid integer for " + std::string(option) + ": " +
                                    std::string(value));
    }
    return result;
}
} // namespace

Options parse_options(std::span<const std::string_view> arguments) {
    Options options;
    bool dimensions_or_steps = false;
    for (std::size_t i = 0; i < arguments.size(); ++i) {
        const auto argument = arguments[i];
        if (argument == "--help") {
            options.help = true;
            continue;
        }
        if (argument == "--version") {
            options.version = true;
            continue;
        }
        if (argument == "--empty") {
            options.empty = true;
            continue;
        }
        if (argument == "--headless") {
            options.headless = true;
            continue;
        }
        if (argument == "--benchmark") {
            options.headless = true;
            options.benchmark = true;
            continue;
        }
        if (argument != "--seed" && argument != "--width" && argument != "--height" &&
            argument != "--steps") {
            throw std::invalid_argument("Unknown option: " + std::string(argument));
        }
        if (++i == arguments.size()) {
            throw std::invalid_argument("Missing value for " + std::string(argument));
        }
        if (argument == "--seed") {
            options.seed = number<std::uint32_t>(argument, arguments[i]);
        } else {
            dimensions_or_steps = true;
            const auto value = number<int>(argument, arguments[i]);
            if (argument == "--width") {
                options.width = value;
            }
            if (argument == "--height") {
                options.height = value;
            }
            if (argument == "--steps") {
                options.steps = value;
            }
        }
    }
    if (options.width < 1 || options.width > simulation::World::max_width || options.height < 1 ||
        options.height > simulation::World::max_height || options.steps < 0 ||
        options.steps > 100000) {
        throw std::invalid_argument("Width: 1..512; height: 1..256; steps: 0..100000");
    }
    if (dimensions_or_steps && !options.headless && !options.help) {
        throw std::invalid_argument(
            "--width, --height, and --steps require --headless or --benchmark");
    }
    if (options.version && arguments.size() != 1) {
        throw std::invalid_argument("Use --version on its own");
    }
    return options;
}

} // namespace dunefall
