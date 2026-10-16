#include "dunefall/app.hpp"
#include "dunefall/simulation/scenes.hpp"
#include "dunefall/simulation/world.hpp"
#include "dunefall/terminal/input.hpp"
#include "dunefall/terminal/renderer.hpp"
#include "dunefall/terminal/session.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

namespace dunefall {
namespace {
using simulation::Material;
using simulation::World;
using terminal::Key;

void paint(World& world, const terminal::View& view) {
    for (int dy = -view.radius; dy <= view.radius; ++dy) {
        for (int dx = -view.radius; dx <= view.radius; ++dx) {
            const int x = view.cursor_x + dx;
            const int y = view.cursor_y + dy;
            if (!world.contains(x, y) || dx * dx + dy * dy > view.radius * view.radius) {
                continue;
            }
            // Painting particles fills empty cells; only erase can remove walls.
            if (view.material == Material::empty || view.material == Material::wall ||
                world.at(x, y) == Material::empty) {
                world.set(x, y, view.material);
            }
        }
    }
}
} // namespace

int play(std::uint32_t seed, bool empty) {
    terminal::Session session;
    const auto initial = session.size();
    World world(std::clamp((initial.columns - 2) / 2, 20, 120),
                std::clamp(initial.rows - 6, 10, 44), seed);
    if (!empty) {
        simulation::load_demo(world);
    }
    terminal::View view;
    view.cursor_x = world.width() / 2;
    view.cursor_y = std::min(3, world.height() - 1);
    terminal::Input input;
    terminal::Renderer renderer;
    bool quit = false;
    using Clock = std::chrono::steady_clock;
    constexpr auto tick = std::chrono::nanoseconds(1'000'000'000 / 30);
    constexpr auto frame = std::chrono::nanoseconds(1'000'000'000 / 30);
    auto previous = Clock::now();
    auto accumulated = Clock::duration::zero();
    while (!quit && session.signal() == 0) {
        const auto start = Clock::now();
        const auto size = session.size();
        for (const auto key : input.feed(session.read_input())) {
            switch (key) {
            case Key::up:
                --view.cursor_y;
                break;
            case Key::down:
                ++view.cursor_y;
                break;
            case Key::left:
                --view.cursor_x;
                break;
            case Key::right:
                ++view.cursor_x;
                break;
            case Key::paint:
                if (!view.help && terminal::usable(size)) {
                    paint(world, view);
                }
                break;
            case Key::sand:
                view.material = Material::sand;
                break;
            case Key::wall:
                view.material = Material::wall;
                break;
            case Key::water:
                view.material = Material::water;
                break;
            case Key::erase:
                view.material = Material::empty;
                break;
            case Key::pause:
                view.paused = !view.paused;
                break;
            case Key::clear:
                world.clear();
                view.spraying = false;
                break;
            case Key::demo:
                simulation::load_demo(world);
                view.spraying = false;
                break;
            case Key::step:
                if (view.paused && !view.help && terminal::usable(size)) {
                    world.step();
                }
                break;
            case Key::spray:
                view.spraying = !view.spraying;
                break;
            case Key::smaller:
                view.radius = std::max(0, view.radius - 1);
                break;
            case Key::larger:
                view.radius = std::min(5, view.radius + 1);
                break;
            case Key::help:
                view.help = !view.help;
                break;
            case Key::quit:
                quit = true;
                break;
            }
            view.cursor_x = std::clamp(view.cursor_x, 0, world.width() - 1);
            view.cursor_y = std::clamp(view.cursor_y, 0, world.height() - 1);
        }
        accumulated += std::min(start - previous, Clock::duration(std::chrono::milliseconds(100)));
        previous = start;
        if (view.paused || view.help || !terminal::usable(size)) {
            accumulated = Clock::duration::zero();
        } else {
            // At most three catch-up steps after a slow frame; rendering cannot
            // accelerate physics, and long stalls cannot create a runaway loop.
            while (accumulated >= tick) {
                if (view.spraying) {
                    paint(world, view);
                }
                world.step();
                accumulated -= tick;
            }
        }
        session.write(renderer.draw(world, view, size));
        std::this_thread::sleep_until(start + frame);
    }
    return session.signal() == 0 ? 0 : 128 + session.signal();
}

} // namespace dunefall
