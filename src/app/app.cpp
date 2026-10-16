#include "dunefall/app.hpp"

#include <charconv>
#include <exception>
#include <iostream>
#include <string_view>

namespace dunefall {

int run(int argc, char* argv[]) {
    std::uint32_t seed = 42;
    bool empty = false;
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument{argv[i]};
        if (argument == "--help") {
            std::cout
                << "Usage: dunefall [--empty] [--seed NUMBER] [--help | --version]\n\n"
                << "A tiny world of sand and beautiful chaos.\n\n"
                << "  --empty        Start with an empty world instead of the demo\n"
                << "  --seed NUMBER  Reproduce a simulation (default: 42)\n"
                << "  --help         Show this message\n"
                << "  --version      Show the project version\n\n"
                << "Arrows/hjkl move | Space paint | Enter spray | 1 sand | 3 wall\n"
                << "0 erase | +/- brush | P pause | N step | C clear | D demo | ? help | Q quit\n";
            return 0;
        }
        if (argument == "--version" && argc == 2) {
            std::cout << "Dunefall " << DUNEFALL_VERSION << '\n';
            return 0;
        }
        if (argument == "--empty") {
            empty = true;
            continue;
        }
        if (argument == "--seed" && i + 1 < argc) {
            const std::string_view value{argv[++i]};
            const auto [end, error] =
                std::from_chars(value.data(), value.data() + value.size(), seed);
            if (error == std::errc{} && end == value.data() + value.size()) {
                continue;
            }
            std::cerr << "Seed must be an unsigned 32-bit integer.\n";
            return 2;
        }
        std::cerr << "Unsupported or incomplete option: " << argument << ". Run --help.\n";
        return 2;
    }
    try {
        return play(seed, empty);
    } catch (const std::exception& error) {
        std::cerr << "Dunefall: " << error.what() << '\n';
        return 1;
    }
}

} // namespace dunefall
