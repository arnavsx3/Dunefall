#include "dunefall/app.hpp"
#include "dunefall/app/options.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace dunefall {

int run(int argc, char* argv[]) {
    std::vector<std::string_view> arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.emplace_back(argv[i]);
    }
    Options options;
    try {
        options = parse_options(arguments);
    } catch (const std::invalid_argument& error) {
        std::cerr << "Dunefall: " << error.what() << ". Run --help.\n";
        return 2;
    }
    if (options.help) {
        std::cout << "Usage: dunefall [OPTIONS]\n\n"
                  << "A tiny world of sand, water, and beautiful chaos.\n\n"
                  << "  --empty        Start empty instead of loading the demo\n"
                  << "  --seed N       Unsigned 32-bit simulation seed (default: 42)\n"
                  << "  --headless     Print an ASCII snapshot without a terminal\n"
                  << "  --benchmark    Measure physics throughput without rendering\n"
                  << "  --width N      Headless world width: 1..512 (default: 64)\n"
                  << "  --height N     Headless world height: 1..256 (default: 24)\n"
                  << "  --steps N      Headless ticks: 0..100000 (default: 120)\n"
                  << "  --help         Show this message\n"
                  << "  --version      Show the project version\n\n"
                  << "Arrows/hjkl move | Space paint | Enter spray | 1 sand | 2 water | 3 wall\n"
                  << "0 erase | +/- brush | P pause | N step | C clear | D demo | ? help | Q quit\n"
                  << "Headless legend: . empty, s sand, w water, # wall.\n";
        return 0;
    }
    if (options.version) {
        std::cout << "Dunefall " << DUNEFALL_VERSION << '\n';
        return 0;
    }
    try {
        return options.headless ? run_headless(options, std::cout)
                                : play(options.seed, options.empty);
    } catch (const std::exception& error) {
        std::cerr << "Dunefall: " << error.what() << '\n';
        return 1;
    }
}

} // namespace dunefall
