#include "dunefall/simulation/scenes.hpp"
#include "dunefall/terminal/renderer.hpp"

#include <iostream>
#include <stdexcept>

using namespace dunefall;

int main() {
    try {
        simulation::World world(100, 60);
        terminal::View view;
        view.cursor_x = 99;
        view.cursor_y = 59;
        const auto area = terminal::viewport(world, view, {50, 18});
        if (area.x != 76 || area.y != 48 || area.width != 24 || area.height != 12) {
            throw std::runtime_error("Viewport must crop around a clamped cursor");
        }
        if (terminal::usable({41, 12}) || terminal::usable({42, 11}) ||
            !terminal::usable({42, 12})) {
            throw std::runtime_error("Minimum terminal size must be enforced");
        }
        terminal::Renderer renderer;
        const auto first = renderer.draw(world, view, {80, 24});
        if (first.find("DUNEFALL") == std::string::npos ||
            !renderer.draw(world, view, {80, 24}).empty()) {
            throw std::runtime_error("Unchanged frames must produce no output");
        }
        world.step();
        const auto changed = renderer.draw(world, view, {80, 24});
        if (changed.empty() || changed.size() >= first.size() / 2) {
            throw std::runtime_error("Tick-only changes should redraw just the status row");
        }
        if (renderer.draw(world, view, {30, 8}).find("Terminal too small") == std::string::npos) {
            throw std::runtime_error("Small terminals must show a useful message");
        }
        view.help = true;
        const auto help = renderer.draw(world, view, {42, 12});
        if (help.find("Q / Ctrl+C quit") == std::string::npos ||
            help.find("2 water") == std::string::npos) {
            throw std::runtime_error("Help must fit even the minimum terminal size");
        }
        if (world.count(simulation::Material::empty) != 6000) {
            throw std::runtime_error("Rendering and resizing must not modify the world");
        }
        simulation::World sample(64, 24, 42);
        simulation::load_demo(sample);
        terminal::Renderer cached;
        terminal::View sample_view;
        std::size_t full_bytes = 0;
        std::size_t changed_bytes = 0;
        for (int i = 0; i < 180; ++i) {
            sample.step();
            full_bytes += terminal::render(sample, sample_view, {130, 30}).size();
            changed_bytes += cached.draw(sample, sample_view, {130, 30}).size();
        }
        if (changed_bytes >= full_bytes) {
            throw std::runtime_error("Representative animation must reduce terminal output");
        }
        std::cout << "180-frame output: full=" << full_bytes << " changed=" << changed_bytes
                  << '\n';
        std::cout << "Renderer: viewport, minimum size, caching, compact help, and read-only "
                     "access passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
