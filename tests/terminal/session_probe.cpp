#include "dunefall/terminal/session.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

int main() {
    try {
        dunefall::terminal::Session session;
        session.write("SESSION_READY");
        while (session.signal() == 0) {
            const auto input = session.read_input();
            if (input.find('q') != std::string::npos) {
                return 0;
            }
            if (input.find('x') != std::string::npos) {
                throw std::runtime_error("Probe exception");
            }
            if (input.find('s') != std::string::npos) {
                const auto size = session.size();
                session.write("SIZE=" + std::to_string(size.columns) + "x" +
                              std::to_string(size.rows));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return 128 + session.signal();
    } catch (...) {
        return 1;
    }
}
