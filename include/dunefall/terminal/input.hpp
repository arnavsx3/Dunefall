#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dunefall::terminal {

enum class Key {
    up,
    down,
    left,
    right,
    paint,
    sand,
    water,
    wall,
    erase,
    pause,
    clear,
    demo,
    step,
    spray,
    smaller,
    larger,
    help,
    quit
};

class Input {
  public:
    std::vector<Key> feed(std::string_view bytes);

  private:
    std::string pending_;
};

} // namespace dunefall::terminal
