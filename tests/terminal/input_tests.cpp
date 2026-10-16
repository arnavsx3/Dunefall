#include "dunefall/terminal/input.hpp"

#include <iostream>
#include <stdexcept>

using dunefall::terminal::Input;
using dunefall::terminal::Key;

int main() {
    try {
        Input input;
        if (!input.feed("\x1b").empty() || !input.feed("[").empty() ||
            input.feed("A") != std::vector{Key::up}) {
            throw std::runtime_error("Fragmented arrows must survive across reads");
        }
        if (input.feed("\x1b[B\x1bOC\x1b[1;5D") != std::vector{Key::down, Key::right, Key::left}) {
            throw std::runtime_error("CSI, application arrows, and modifiers must decode");
        }
        if (input.feed("1230 pcd\r-+?nq") !=
            std::vector{Key::sand, Key::water, Key::wall, Key::erase, Key::paint, Key::pause,
                        Key::clear, Key::demo, Key::spray, Key::smaller, Key::larger, Key::help,
                        Key::step, Key::quit}) {
            throw std::runtime_error("Application shortcuts must decode in order");
        }
        if (input.feed("\x1b[3~\t\x1bX").size() != 0 ||
            input.feed("hjkl") != std::vector{Key::left, Key::down, Key::up, Key::right}) {
            throw std::runtime_error("Unknown sequences must not leak into shortcuts");
        }
        std::cout << "Input: fragmented sequences, arrows, shortcuts, and unknown keys passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
