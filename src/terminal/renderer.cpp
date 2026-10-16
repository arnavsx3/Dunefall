#include "dunefall/terminal/renderer.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>

namespace dunefall::terminal {
namespace {
using simulation::Material;
std::string_view label(Material material) {
    switch (material) {
    case Material::sand:
        return "SAND";
    case Material::water:
        return "WATER";
    case Material::wall:
        return "WALL";
    case Material::empty:
        return "ERASE";
    }
    return "?";
}
std::string_view color(Material material) {
    switch (material) {
    case Material::sand:
        return "\x1b[38;5;222m";
    case Material::water:
        return "\x1b[38;5;75m";
    case Material::wall:
        return "\x1b[38;5;245m";
    case Material::empty:
        return "\x1b[38;5;238m";
    }
    return "\x1b[39m";
}
std::string_view tile(Material material) {
    switch (material) {
    case Material::sand:
        return "░░";
    case Material::water:
        return "≈≈";
    case Material::wall:
        return "██";
    case Material::empty:
        return "  ";
    }
    return "  ";
}
void line(std::string& result, int row, std::string text, Size size) {
    if (row > size.rows) {
        return;
    }
    if (text.size() > static_cast<std::size_t>(size.columns)) {
        text.resize(static_cast<std::size_t>(size.columns));
    }
    result += "\x1b[" + std::to_string(row) + ";1H\x1b[0;38;5;252;48;5;234m\x1b[2K";
    if (row == 1) {
        result += "\x1b[1;38;5;117m";
    }
    result += text;
}
} // namespace

bool usable(Size size) noexcept { return size.columns >= 42 && size.rows >= 12; }

Viewport viewport(const simulation::World& world, const View& view, Size size) {
    const int width = std::min(world.width(), std::max(1, (size.columns - 2) / 2));
    const int height = std::min(world.height(), std::max(1, size.rows - 6));
    return {std::clamp(view.cursor_x - width / 2, 0, world.width() - width),
            std::clamp(view.cursor_y - height / 2, 0, world.height() - height), width, height};
}

std::string render(const simulation::World& world, const View& view, Size size) {
    std::string result;
    result.reserve(static_cast<std::size_t>(std::max(size.rows, 1)) * 200);
    line(result, 1, "DUNEFALL / a tiny world of beautiful chaos", size);
    if (!usable(size)) {
        line(result, 3, "Terminal too small. Simulation paused.", size);
        line(result, 4, "Resize to at least 42 columns x 12 rows.", size);
        line(result, 6, "Q quit | Ctrl+C exit", size);
        result += "\x1b[J";
        return result;
    }
    if (view.help) {
        constexpr std::array help{"THE MATERIAL LAB / controls",
                                  "",
                                  "Arrows / h j k l   Move brush",
                                  "Space paint | Enter toggle spray",
                                  "1 sand | 2 water | 3 wall | 0 erase",
                                  "+ / - brush | P pause | N single step",
                                  "C clear | D reload demo",
                                  "? close help | Q / Ctrl+C quit",
                                  "Physics pauses while help is open."};
        for (std::size_t i = 0; i < help.size(); ++i) {
            line(result, static_cast<int>(i) + 3, help[i], size);
        }
        result += "\x1b[J";
        return result;
    }
    const auto sand = world.count(Material::sand);
    std::string status = std::string(view.paused ? "PAUSED" : "RUNNING") + " | " +
                         std::string(label(view.material)) + " | brush " +
                         std::to_string(2 * view.radius + 1) + " | sand " + std::to_string(sand) +
                         " | water " + std::to_string(world.count(Material::water)) +
                         (view.spraying ? " | SPRAY ON" : "") + " | tick " +
                         std::to_string(world.ticks());
    line(result, 2, status, size);
    const auto area = viewport(world, view, size);
    const std::string border =
        "+" + std::string(static_cast<std::size_t>(area.width * 2), '-') + "+";
    line(result, 3, border, size);
    for (int y = 0; y < area.height; ++y) {
        line(result, y + 4, "|", size);
        std::optional<Material> previous_material;
        for (int x = 0; x < area.width; ++x) {
            const int world_x = x + area.x;
            const int world_y = y + area.y;
            const auto material = world.at(world_x, world_y);
            const bool cursor = world_x == view.cursor_x && world_y == view.cursor_y;
            if (!previous_material || *previous_material != material) {
                result += color(material);
                previous_material = material;
            }
            if (cursor) {
                result += "\x1b[38;5;231;48;5;60m<>\x1b[48;5;234m";
                previous_material.reset();
            } else {
                result += tile(material);
            }
        }
        result += "\x1b[38;5;245m|";
    }
    line(result, area.height + 4, border, size);
    line(result, area.height + 5, "Arrows move | Space paint | Enter spray", size);
    line(result, area.height + 6, "1/2/3/0 material | P pause | ? help Q quit", size);
    result += "\x1b[J";
    return result;
}

std::string Renderer::draw(const simulation::World& world, const View& view, Size size) {
    const auto full = render(world, view, size);
    std::vector<std::size_t> starts;
    // Every row begins with an absolute cursor address. Styling and tile codes
    // do not contain this suffix, so row boundaries stay independent of UTF-8.
    for (auto marker = full.find(";1H"); marker != std::string::npos;
         marker = full.find(";1H", marker + 3)) {
        starts.push_back(full.rfind("\x1b[", marker));
    }
    std::vector<std::string> rows;
    for (std::size_t i = 0; i < starts.size(); ++i) {
        const auto end = i + 1 < starts.size() ? starts[i + 1] : full.size();
        rows.push_back(full.substr(starts[i], end - starts[i]));
    }
    const bool reset = size.columns != previous_size_.columns || size.rows != previous_size_.rows ||
                       rows.size() != previous_rows_.size();
    std::string changed = reset ? "\x1b[2J" : "";
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (reset || rows[i] != previous_rows_[i]) {
            changed += rows[i];
        }
    }
    previous_size_ = size;
    previous_rows_ = std::move(rows);
    return changed;
}

} // namespace dunefall::terminal
