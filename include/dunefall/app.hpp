#pragma once

#include <cstdint>

namespace dunefall {

// Application boundary; simulation and terminal code will live in separate modules.
int run(int argc, char* argv[]);
int play(std::uint32_t seed, bool empty);

} // namespace dunefall
