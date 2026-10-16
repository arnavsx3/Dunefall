#pragma once

#include "dunefall/simulation/world.hpp"
#include "dunefall/terminal/session.hpp"

#include <string>

namespace dunefall::terminal {

struct View {
    int cursor_x = 0;
    int cursor_y = 0;
    int radius = 0;
    simulation::Material material = simulation::Material::sand;
    bool paused = false;
    bool spraying = false;
    bool help = false;
};

struct Viewport {
    int x;
    int y;
    int width;
    int height;
};

[[nodiscard]] bool usable(Size size) noexcept;
[[nodiscard]] Viewport viewport(const simulation::World& world, const View& view, Size size);
[[nodiscard]] std::string render(const simulation::World& world, const View& view, Size size);

} // namespace dunefall::terminal
