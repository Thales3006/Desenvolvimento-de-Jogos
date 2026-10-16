#include "TileMap.hpp"
#include "Dungeon2D.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <random>
#include <sstream>

namespace {

/// Carves (sets to EMPTY_TILE) an inclusive rectangle, clamped to the matrix.
void carveRect(std::vector<int>& cells, int width, int height, const Rect& r) {
    const int left   = std::max(0, r.left);
    const int top    = std::max(0, r.top);
    const int right  = std::min(width  - 1, r.right);
    const int bottom = std::min(height - 1, r.bottom);
    for (int row = top; row <= bottom; ++row) {
        for (int col = left; col <= right; ++col) {
            cells[static_cast<std::size_t>(row) * width + col] = EMPTY_TILE;
        }
    }
}

/// Walks the BSP tree, carving the room held in every leaf.
void carveLeaves(const BSPNode& node, std::vector<int>& cells, int width, int height) {
    if (node.isLeaf()) {
        carveRect(cells, width, height, node.room);
        return;
    }
    if (node.left)  carveLeaves(*node.left,  cells, width, height);
    if (node.right) carveLeaves(*node.right, cells, width, height);
}

} // namespace

// --- Construction from geometry --------------------------------------------

void TileMap::loadFromDungeon(const Dungeon2D& dungeon) {
    m_width  = dungeon.width;
    m_height = dungeon.height;
    m_cells.assign(static_cast<std::size_t>(m_width) * m_height, WALL_TILE);

    if (dungeon.root) {
        carveLeaves(*dungeon.root, m_cells, m_width, m_height);
    }
    for (const Corridor& c : dungeon.corridors) {
        carveRect(m_cells, m_width, m_height, c);
    }
}

// --- CSV persistence -------------------------------------------------------

bool TileMap::saveCSV(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;

    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            if (col > 0) out << ',';
            out << m_cells[static_cast<std::size_t>(row) * m_width + col];
        }
        out << '\n';
    }
    return static_cast<bool>(out);
}

bool TileMap::loadCSV(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    // Parse everything first; only commit if the whole file is well formed,
    // so a failure leaves the map unchanged.
    std::vector<std::vector<int>> rows;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<int> cells;
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, ',')) {
            try {
                cells.push_back(std::stoi(field));
            } catch (...) {
                return false;   // non-integer field
            }
        }
        rows.push_back(std::move(cells));
    }

    if (rows.empty()) return false;
    const std::size_t cols = rows.front().size();
    if (cols == 0) return false;
    for (const auto& r : rows) {
        if (r.size() != cols) return false;   // ragged matrix -> reject
    }

    m_height = static_cast<int>(rows.size());
    m_width  = static_cast<int>(cols);
    m_cells.clear();
    m_cells.reserve(static_cast<std::size_t>(m_width) * m_height);
    for (const auto& r : rows) {
        m_cells.insert(m_cells.end(), r.begin(), r.end());
    }
    return true;
}

// --- Queries ---------------------------------------------------------------

int TileMap::width()  const noexcept { return m_width; }
int TileMap::height() const noexcept { return m_height; }

int TileMap::at(int col, int row) const {
    assert(col >= 0 && col < m_width && row >= 0 && row < m_height &&
           "at(): indices out of range");
    return m_cells[static_cast<std::size_t>(row) * m_width + col];
}

bool TileMap::isWall(int col, int row) const {
    return at(col, row) == WALL_TILE;
}

const int* TileMap::data() const noexcept {
    return m_cells.data();
}

// --- Continuous <-> discrete translation -----------------------------------

void TileMap::toIndices(const Vector2D& world, int& col, int& row) const noexcept {
    col = static_cast<int>(std::floor(world.x / TILE_SIZE));
    row = static_cast<int>(std::floor(world.y / TILE_SIZE));
}

Vector2D TileMap::toWorld(int col, int row) const noexcept {
    return {(static_cast<float>(col) + 0.5f) * TILE_SIZE,
            (static_cast<float>(row) + 0.5f) * TILE_SIZE};
}

void TileMap::randomEmptyCell(int& col, int& row) const {
    std::vector<int> empties;
    empties.reserve(m_cells.size());
    for (int i = 0; i < static_cast<int>(m_cells.size()); ++i) {
        if (m_cells[i] == EMPTY_TILE) empties.push_back(i);
    }
    assert(!empties.empty() && "randomEmptyCell(): no empty cell exists");

    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(0, static_cast<int>(empties.size()) - 1);
    const int index = empties[dist(rng)];
    col = index % m_width;
    row = index / m_width;
}
