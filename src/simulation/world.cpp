#include "dunefall/simulation/world.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace dunefall::simulation {

World::World(int width, int height, std::uint32_t seed)
    : width_(width), height_(height), seed_(seed), random_(seed) {
    if (width < 1 || height < 1 || width > max_width || height > max_height) {
        throw std::invalid_argument("World dimensions must be within 1..512 by 1..256");
    }
    const auto size = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    cells_.resize(size, Material::empty);
    updated_.resize(size, 0);
}

bool World::contains(int x, int y) const noexcept {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
}

std::size_t World::index(int x, int y) const noexcept {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
           static_cast<std::size_t>(x);
}

Material World::at(int x, int y) const {
    if (!contains(x, y)) {
        throw std::out_of_range("Cell is outside the world");
    }
    return cells_[index(x, y)];
}

void World::set(int x, int y, Material material) {
    if (!contains(x, y)) {
        throw std::out_of_range("Cell is outside the world");
    }
    cells_[index(x, y)] = material;
}

std::size_t World::count(Material material) const {
    return static_cast<std::size_t>(std::count(cells_.begin(), cells_.end(), material));
}

void World::clear() {
    std::fill(cells_.begin(), cells_.end(), Material::empty);
    std::fill(updated_.begin(), updated_.end(), 0);
    tick_ = 0;
    random_.seed(seed_);
}

bool World::move(int x, int y, int target_x, int target_y) {
    if (!contains(target_x, target_y)) {
        return false;
    }
    const auto from = index(x, y);
    const auto to = index(target_x, target_y);
    const bool sinking = cells_[from] == Material::sand && cells_[to] == Material::water;
    if (cells_[to] != Material::empty && !sinking) {
        return false;
    }
    if (sinking && updated_[to] == tick_) {
        return false; // A displaced water particle must not move twice this tick.
    }
    std::swap(cells_[from], cells_[to]);
    updated_[from] = tick_;
    updated_[to] = tick_;
    return true;
}

void World::step() {
    if (tick_ == std::numeric_limits<std::uint64_t>::max()) {
        std::fill(updated_.begin(), updated_.end(), 0);
        tick_ = 0;
    }
    ++tick_;
    // Bottom-up updates let stacks settle. Destination stamps prevent a particle
    // from moving twice when a diagonal move lands ahead of the scan direction.
    for (int y = height_ - 1; y >= 0; --y) {
        const bool reverse = (random_() & 1U) != 0;
        for (int column = 0; column < width_; ++column) {
            const int x = reverse ? width_ - 1 - column : column;
            const auto material = cells_[index(x, y)];
            if ((material != Material::sand && material != Material::water) ||
                updated_[index(x, y)] == tick_) {
                continue;
            }
            const int direction = (random_() & 1U) != 0 ? 1 : -1;
            if (move(x, y, x, y + 1) || move(x, y, x + direction, y + 1)) {
                continue;
            }
            if (move(x, y, x - direction, y + 1)) {
                continue;
            }
            if (material == Material::water && !move(x, y, x + direction, y)) {
                move(x, y, x - direction, y);
            }
        }
    }
}

} // namespace dunefall::simulation
