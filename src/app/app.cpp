#include "dunefall/app.hpp"

#include <iostream>
#include <string_view>

namespace dunefall {

int run(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Dunefall " << DUNEFALL_VERSION << "\n"
                  << "Workspace ready. The simulation is not implemented yet.\n"
                  << "Run with --help for available options.\n";
        return 0;
    }

    const std::string_view argument{argv[1]};
    if (argc == 2 && argument == "--help") {
        std::cout << "Usage: dunefall [--help | --version]\n\n"
                  << "A tiny world of sand, water, and beautiful chaos.\n"
                  << "This initial workspace does not include the game yet.\n\n"
                  << "  --help     Show this message\n"
                  << "  --version  Show the project version\n";
        return 0;
    }
    if (argc == 2 && argument == "--version") {
        std::cout << "Dunefall " << DUNEFALL_VERSION << "\n";
        return 0;
    }

    std::cerr << "Unsupported arguments. Usage: dunefall [--help | --version]\n";
    return 2;
}

} // namespace dunefall
