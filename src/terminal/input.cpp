#include "dunefall/terminal/input.hpp"

#include <optional>

namespace dunefall::terminal {
namespace {
std::optional<Key> character(char value) {
    switch (value) {
    case 'k':
        return Key::up;
    case 'j':
        return Key::down;
    case 'h':
        return Key::left;
    case 'l':
        return Key::right;
    case ' ':
        return Key::paint;
    case '1':
        return Key::sand;
    case '2':
        return Key::water;
    case '3':
        return Key::wall;
    case '0':
    case 'e':
        return Key::erase;
    case 'p':
        return Key::pause;
    case 'c':
        return Key::clear;
    case 'd':
        return Key::demo;
    case 'n':
        return Key::step;
    case '\r':
    case '\n':
        return Key::spray;
    case '-':
    case '[':
        return Key::smaller;
    case '+':
    case '=':
    case ']':
        return Key::larger;
    case '?':
        return Key::help;
    case 'q':
    case 'Q':
        return Key::quit;
    default:
        return std::nullopt;
    }
}
} // namespace

std::vector<Key> Input::feed(std::string_view bytes) {
    pending_.append(bytes);
    std::vector<Key> result;
    while (!pending_.empty()) {
        if (pending_.front() != '\x1b') {
            if (const auto key = character(pending_.front())) {
                result.push_back(*key);
            }
            pending_.erase(0, 1);
            continue;
        }
        if (pending_.size() == 1) {
            break;
        }
        if (pending_[1] != '[' && pending_[1] != 'O') {
            pending_.erase(0, 2);
            continue;
        }
        const auto final = pending_.find_first_of(
            "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 2);
        if (final == std::string::npos) {
            if (pending_.size() > 32) {
                pending_.clear();
            }
            break;
        }
        switch (pending_[final]) {
        case 'A':
            result.push_back(Key::up);
            break;
        case 'B':
            result.push_back(Key::down);
            break;
        case 'C':
            result.push_back(Key::right);
            break;
        case 'D':
            result.push_back(Key::left);
            break;
        default:
            break;
        }
        pending_.erase(0, final + 1);
    }
    return result;
}

} // namespace dunefall::terminal
