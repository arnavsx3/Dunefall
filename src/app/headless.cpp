#include "dunefall/app/options.hpp"
#include "dunefall/simulation/scenes.hpp"

#include <chrono>
#include <iomanip>

namespace dunefall {

int run_headless(const Options& options, std::ostream& output) {
    simulation::World world(options.width, options.height, options.seed);
    if (!options.empty) {
        simulation::load_demo(world);
    }
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < options.steps; ++i) {
        world.step();
    }
    const auto duration = std::chrono::steady_clock::now() - start;
    output << "Dunefall | seed " << options.seed << " | " << world.width() << 'x' << world.height()
           << " | ticks " << world.ticks() << '\n';
    if (!options.benchmark) {
        constexpr std::string_view symbols = ".sw#";
        for (int y = 0; y < world.height(); ++y) {
            for (int x = 0; x < world.width(); ++x) {
                output << symbols[static_cast<std::size_t>(world.at(x, y))];
            }
            output << '\n';
        }
    }
    output << "sand=" << world.count(simulation::Material::sand)
           << " water=" << world.count(simulation::Material::water)
           << " walls=" << world.count(simulation::Material::wall) << '\n';
    if (options.benchmark) {
        const auto seconds = std::chrono::duration<double>(duration).count();
        output << std::fixed << std::setprecision(2) << "elapsed_ms=" << seconds * 1000.0
               << " steps_per_second=" << (seconds > 0.0 ? options.steps / seconds : 0.0) << '\n';
    }
    return 0;
}

} // namespace dunefall
