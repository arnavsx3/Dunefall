#include "dunefall/simulation/scenes.hpp"

#include <algorithm>

namespace dunefall::simulation {

void load_demo(World& world) {
    world.clear();
    const int width = world.width();
    const int height = world.height();
    for (int x = 0; x < width; ++x) {
        world.set(x, height - 1, Material::wall);
    }
    if (width < 12 || height < 8) {
        if (height > 1) {
            world.set(width / 2, 0, Material::sand);
        }
        return;
    }
    const int shelf = height / 2;
    for (int x = 2; x < width / 3; ++x) {
        world.set(x, shelf, Material::wall);
        world.set(width - 1 - x, shelf, Material::wall);
    }
    for (int y = std::max(0, shelf - 3); y < shelf; ++y) {
        world.set(2, y, Material::wall);
        for (int x = 3; x < width / 3; ++x) {
            world.set(x, y, Material::water);
        }
    }
    for (int y = std::max(shelf + 1, height - 4); y < height - 1; ++y) {
        for (int x = width / 4; x < 3 * width / 4; ++x) {
            world.set(x, y, Material::water);
        }
    }
    for (int y = 0; y < height / 3; ++y) {
        for (int x = width / 2 - y / 2; x <= width / 2 + y / 2; ++x) {
            world.set(x, y, Material::sand);
        }
    }
}

} // namespace dunefall::simulation
