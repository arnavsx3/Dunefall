#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace dunefall::simulation {

enum class Material : std::uint8_t { empty, sand, water, wall };

class World {
  public:
    static constexpr int max_width = 512;
    static constexpr int max_height = 256;

    explicit World(int width, int height, std::uint32_t seed = 42);

    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] bool contains(int x, int y) const noexcept;
    [[nodiscard]] Material at(int x, int y) const;
    [[nodiscard]] std::size_t count(Material material) const;
    [[nodiscard]] std::uint64_t ticks() const noexcept { return tick_; }
    [[nodiscard]] const std::vector<Material>& cells() const noexcept { return cells_; }

    void set(int x, int y, Material material);
    void clear();
    void step();

  private:
    [[nodiscard]] std::size_t index(int x, int y) const noexcept;
    bool move(int x, int y, int target_x, int target_y);

    int width_;
    int height_;
    std::uint32_t seed_;
    std::mt19937 random_;
    std::vector<Material> cells_;
    std::vector<std::uint64_t> updated_;
    std::uint64_t tick_ = 0;
};

} // namespace dunefall::simulation
