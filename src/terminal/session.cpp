#include "dunefall/terminal/session.hpp"

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

namespace dunefall::terminal {
namespace {
volatile std::sig_atomic_t caught_signal = 0;
constexpr std::array signals{SIGINT, SIGTERM, SIGHUP, SIGQUIT};
void request_exit(int signal) { caught_signal = signal; }
} // namespace

Session::Session() {
    if (!::isatty(STDIN_FILENO) || !::isatty(STDOUT_FILENO)) {
        throw std::runtime_error(
            "Interactive mode needs a terminal. Use --help for other options.");
    }
    const char* term = std::getenv("TERM");
    if (term != nullptr && std::string_view(term) == "dumb") {
        throw std::runtime_error("Dunefall needs an ANSI-compatible terminal (TERM is dumb).");
    }
    if (::tcgetattr(STDIN_FILENO, &saved_) < 0) {
        throw std::runtime_error("Could not read terminal settings");
    }
    flags_ = ::fcntl(STDIN_FILENO, F_GETFL);
    if (flags_ < 0) {
        throw std::runtime_error("Could not read terminal input flags");
    }
    try {
        caught_signal = 0;
        for (std::size_t i = 0; i < signals.size(); ++i) {
            struct sigaction action{};
            action.sa_handler = request_exit;
            ::sigemptyset(&action.sa_mask);
            if (::sigaction(signals[i], &action, &handlers_[i]) < 0) {
                throw std::runtime_error("Could not install terminal signal handlers");
            }
            ++installed_signals_;
        }
        termios raw = saved_;
        raw.c_lflag &= static_cast<tcflag_t>(~(ECHO | ICANON | IEXTEN));
        raw.c_iflag &= static_cast<tcflag_t>(~(IXON | ICRNL));
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        if (::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
            throw std::runtime_error("Could not enable terminal input");
        }
        configured_ = true;
        if (::fcntl(STDIN_FILENO, F_SETFL, flags_ | O_NONBLOCK) < 0) {
            throw std::runtime_error("Could not enable nonblocking terminal input");
        }
        screen_ = true;
        write("\x1b[?1049h\x1b[?25l\x1b[?7l\x1b[2J");
    } catch (...) {
        restore();
        throw;
    }
}

Session::~Session() { restore(); }

void Session::restore() noexcept {
    if (configured_) {
        ::tcsetattr(STDIN_FILENO, TCSANOW, &saved_);
        ::fcntl(STDIN_FILENO, F_SETFL, flags_);
        configured_ = false;
    }
    if (screen_) {
        constexpr std::string_view ending = "\x1b[0m\x1b[?7h\x1b[?25h\x1b[?1049l";
        // Best effort: cleanup must never throw while unwinding an exception.
        std::size_t sent = 0;
        while (sent < ending.size()) {
            const auto count = ::write(STDOUT_FILENO, ending.data() + sent, ending.size() - sent);
            if (count > 0) {
                sent += static_cast<std::size_t>(count);
            } else if (count < 0 && errno == EINTR) {
                continue;
            } else {
                break;
            }
        }
        screen_ = false;
    }
    while (installed_signals_ > 0) {
        --installed_signals_;
        ::sigaction(signals[installed_signals_], &handlers_[installed_signals_], nullptr);
    }
}

Size Session::size() const noexcept {
    winsize dimensions{};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &dimensions) == 0 && dimensions.ws_col > 0 &&
        dimensions.ws_row > 0) {
        return {dimensions.ws_col, dimensions.ws_row};
    }
    return {80, 24};
}

int Session::signal() const noexcept { return caught_signal; }

std::string Session::read_input() {
    std::string result;
    pollfd descriptor{STDIN_FILENO, POLLIN, 0};
    if (::poll(&descriptor, 1, 0) < 0 && errno != EINTR) {
        throw std::runtime_error("Could not poll terminal input");
    }
    if ((descriptor.revents & (POLLHUP | POLLERR | POLLNVAL)) != 0) {
        caught_signal = SIGHUP;
        return result;
    }
    std::array<char, 256> buffer{};
    // Bound work per frame so key repeat cannot starve simulation or shutdown.
    while (result.size() < 4096) {
        const auto count = ::read(STDIN_FILENO, buffer.data(), buffer.size());
        if (count > 0) {
            result.append(buffer.data(), static_cast<std::size_t>(count));
        } else if (count == 0 || errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            break;
        } else {
            throw std::runtime_error("Could not read terminal input");
        }
    }
    return result;
}

void Session::write(std::string_view text) const {
    while (!text.empty()) {
        const auto count = ::write(STDOUT_FILENO, text.data(), text.size());
        if (count > 0) {
            text.remove_prefix(static_cast<std::size_t>(count));
        } else if (count < 0 && errno == EINTR) {
            if (caught_signal != 0) {
                return;
            }
        } else if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // stdin and stdout can share a nonblocking file description. Wait
            // for output capacity while still responding to shutdown signals.
            pollfd output{STDOUT_FILENO, POLLOUT, 0};
            const auto ready = ::poll(&output, 1, 50);
            if (caught_signal != 0) {
                return;
            }
            if ((ready < 0 && errno != EINTR) ||
                (output.revents & (POLLHUP | POLLERR | POLLNVAL)) != 0) {
                throw std::runtime_error("Terminal output disconnected");
            }
        } else {
            throw std::runtime_error("Could not write terminal output");
        }
    }
}

} // namespace dunefall::terminal
