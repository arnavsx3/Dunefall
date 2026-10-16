#include "dunefall/simulation/scenes.hpp"
#include "dunefall/simulation/world.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

using dunefall::simulation::Material;
using dunefall::simulation::World;

namespace {
void require(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

template <typename Exception, typename Function> void require_throws(Function function) {
    try {
        function();
    } catch (const Exception&) {
        return;
    }
    throw std::runtime_error("Expected exception was not thrown");
}

void bounds() {
    require_throws<std::invalid_argument>([] { World world(0, 5); });
    require_throws<std::invalid_argument>([] { World world(5, -1); });
    require_throws<std::invalid_argument>([] { World world(World::max_width + 1, 1); });
    require_throws<std::invalid_argument>([] { World world(1, World::max_height + 1); });
    World world(5, 3);
    require(world.count(Material::empty) == 15, "World starts empty");
    require(!world.contains(-1, 0) && !world.contains(0, 3), "Bounds reject invalid positions");
    require_throws<std::out_of_range>([&] { (void)world.at(-1, 0); });
    require_throws<std::out_of_range>([&] { world.set(5, 0, Material::sand); });
}

void gravity() {
    World world(5, 6);
    world.set(2, 0, Material::sand);
    world.step();
    require(world.at(2, 1) == Material::sand, "Sand falls exactly one row per step");
    require(world.at(2, 0) == Material::empty, "Source is emptied");
    for (int i = 0; i < 10; ++i) {
        world.step();
    }
    require(world.count(Material::sand) == 1, "Floor conserves particles");
    require(world.at(2, 5) == Material::sand, "Sand stops at the bottom");
}

void collision() {
    World world(5, 4);
    world.set(2, 1, Material::sand);
    world.set(2, 2, Material::wall);
    world.step();
    require(world.at(2, 2) == Material::wall, "Wall is never displaced");
    require(world.at(1, 2) == Material::sand || world.at(3, 2) == Material::sand,
            "Sand moves diagonally around a wall");
    World blocked(1, 2);
    blocked.set(0, 0, Material::sand);
    blocked.set(0, 1, Material::wall);
    blocked.step();
    require(blocked.at(0, 0) == Material::sand, "A blocked narrow column is stable");
    World single(1, 1);
    single.set(0, 0, Material::sand);
    single.step();
    require(single.at(0, 0) == Material::sand, "A one-cell world is safe");
}

void conservation_and_replay() {
    World first(31, 19, 123);
    World second(31, 19, 123);
    for (int x = 0; x < 31; ++x) {
        for (int y = 0; y < 7; ++y) {
            if ((x + y) % 3 != 0) {
                first.set(x, y, Material::sand);
                second.set(x, y, Material::sand);
            }
        }
        first.set(x, 15, Material::wall);
        second.set(x, 15, Material::wall);
    }
    const auto sand = first.count(Material::sand);
    for (int i = 0; i < 200; ++i) {
        first.step();
        second.step();
        require(first.count(Material::sand) == sand, "Sand is conserved across long runs");
        require(first.count(Material::wall) == 31, "Walls stay intact");
        require(first.cells() == second.cells(), "A seed reproduces the same world each tick");
    }
    first.clear();
    require(first.count(Material::empty) == 31 * 19 && first.ticks() == 0,
            "Clear resets all cells and the tick counter");
    first.set(2, 1, Material::sand);
    first.set(2, 2, Material::wall);
    World reset(31, 19, 123);
    reset.set(2, 1, Material::sand);
    reset.set(2, 2, Material::wall);
    first.step();
    reset.step();
    require(first.cells() == reset.cells(), "Clear resets the random sequence");
}

void water_flow() {
    World falling(5, 5);
    falling.set(2, 0, Material::water);
    falling.step();
    require(falling.at(2, 1) == Material::water, "Water falls one row per step");
    World lateral(5, 1);
    lateral.set(2, 0, Material::water);
    lateral.step();
    require(lateral.at(1, 0) == Material::water || lateral.at(3, 0) == Material::water,
            "Blocked water spreads exactly one cell sideways");
    require(lateral.count(Material::water) == 1, "Sideways movement conserves water");
    World sink(1, 3);
    sink.set(0, 0, Material::sand);
    sink.set(0, 1, Material::water);
    sink.set(0, 2, Material::wall);
    sink.step();
    require(sink.at(0, 0) == Material::water && sink.at(0, 1) == Material::sand,
            "Sand sinks through water and displaces it upward once");
    sink.step();
    require(sink.at(0, 1) == Material::sand && sink.count(Material::water) == 1,
            "Water cannot displace sand and blocked columns stay stable");
}

void mixed_conservation() {
    World first(29, 20, 987);
    World second(29, 20, 987);
    dunefall::simulation::load_demo(first);
    dunefall::simulation::load_demo(second);
    const auto sand = first.count(Material::sand);
    const auto water = first.count(Material::water);
    const auto walls = first.count(Material::wall);
    require(sand > 0 && water > 0 && walls > 0, "Demo includes all materials");
    for (int i = 0; i < 1000; ++i) {
        first.step();
        second.step();
        require(first.count(Material::sand) == sand, "Mixed runs conserve sand");
        require(first.count(Material::water) == water, "Mixed runs conserve water");
        require(first.count(Material::wall) == walls, "Mixed runs preserve walls");
        require(first.cells() == second.cells(), "Mixed worlds replay deterministically");
    }
    World basin(8, 8);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (x == 0 || x == 7 || y == 0 || y == 7) {
                basin.set(x, y, Material::wall);
            } else if (y < 4) {
                basin.set(x, y, Material::water);
            }
        }
    }
    const auto contained = basin.count(Material::water);
    for (int i = 0; i < 300; ++i) {
        basin.step();
    }
    require(basin.count(Material::water) == contained && basin.count(Material::wall) == 28,
            "A closed wall basin contains every water particle");
    World tiny(1, 1);
    dunefall::simulation::load_demo(tiny);
    tiny.step();
    require(tiny.at(0, 0) == Material::wall, "Demo is safe at minimum dimensions");
}
} // namespace

int main() {
    try {
        bounds();
        gravity();
        collision();
        conservation_and_replay();
        water_flow();
        mixed_conservation();
        std::cout << "World: bounds, gravity, collisions, conservation, replay, reset, water, "
                     "sinking, and containment passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
