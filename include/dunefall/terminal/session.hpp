#pragma once

#include <array>
#include <csignal>
#include <cstddef>
#include <string>
#include <string_view>
#include <termios.h>

namespace dunefall::terminal {

struct Size {
    int columns;
    int rows;
};

class Session {
  public:
    Session();
    ~Session();
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    [[nodiscard]] Size size() const noexcept;
    [[nodiscard]] int signal() const noexcept;
    std::string read_input();
    void write(std::string_view text) const;

  private:
    void restore() noexcept;
    termios saved_{};
    int flags_ = -1;
    bool configured_ = false;
    bool screen_ = false;
    std::size_t installed_signals_ = 0;
    std::array<struct sigaction, 4> handlers_{};
};

} // namespace dunefall::terminal
