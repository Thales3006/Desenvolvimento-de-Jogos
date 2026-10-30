#include "PathFind.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

PathFind::PathFind(const int* grid, int width, int height)
    : m_grid(grid), m_width(width), m_height(height) {}

namespace {

/// Octile distance: the cost of the cheapest 8-connected path on an empty grid,
/// with orthogonal steps costing 1 and diagonal steps costing sqrt(2). It never
/// overestimates the real cost, so A* with it stays optimal (admissible).
float octile(int dc, int dr) {
    const int adx = std::abs(dc);
    const int ady = std::abs(dr);
    const int lo  = std::min(adx, ady);
    const int hi  = std::max(adx, ady);
    static const float SQRT2 = std::sqrt(2.0f);
    // lo diagonal steps close both axes, the rest are orthogonal.
    return SQRT2 * static_cast<float>(lo) + static_cast<float>(hi - lo);
}

} // namespace

std::vector<Vector2D> PathFind::findPath(const Vector2D& startWorld,
                                         const Vector2D& goalWorld) const {
    // Map the continuous endpoints to cells (Global.hpp convention).
    const int startCol = static_cast<int>(std::floor(startWorld.x / TILE_SIZE));
    const int startRow = static_cast<int>(std::floor(startWorld.y / TILE_SIZE));
    const int goalCol  = static_cast<int>(std::floor(goalWorld.x  / TILE_SIZE));
    const int goalRow  = static_cast<int>(std::floor(goalWorld.y  / TILE_SIZE));

    // A cell is usable only if it is in range and walkable (EMPTY_TILE).
    const auto inRange = [this](int col, int row) {
        return col >= 0 && col < m_width && row >= 0 && row < m_height;
    };
    const auto walkable = [&](int col, int row) {
        return inRange(col, row) && m_grid[row * m_width + col] == EMPTY_TILE;
    };

    // No path can start or end off a walkable in-range cell.
    if (!walkable(startCol, startRow) || !walkable(goalCol, goalRow)) {
        return {};
    }

    const int cellCount   = m_width * m_height;
    const auto indexOf    = [this](int col, int row) { return row * m_width + col; };
    const int startIndex  = indexOf(startCol, startRow);
    const int goalIndex   = indexOf(goalCol, goalRow);

    // Per-cell search state, indexed row-major like the grid.
    std::vector<float> gScore(cellCount, std::numeric_limits<float>::infinity());
    std::vector<int>   cameFrom(cellCount, -1);
    std::vector<bool>  closed(cellCount, false);

    // Open set, ordered by f = g + h (cheapest first). We store the f value so a
    // cell can appear more than once; stale entries are skipped via `closed`.
    using Entry = std::pair<float, int>;   // (f, cell index)
    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> open;

    gScore[startIndex] = 0.0f;
    open.push({octile(goalCol - startCol, goalRow - startRow), startIndex});

    // The eight neighbour offsets; the last four are the diagonals.
    static const int DC[8] = {+1, -1,  0,  0, +1, +1, -1, -1};
    static const int DR[8] = { 0,  0, +1, -1, +1, -1, +1, -1};

    while (!open.empty()) {
        const int current = open.top().second;
        open.pop();

        if (current == goalIndex) {
            break;                       // Reached the goal with optimal cost.
        }
        if (closed[current]) {
            continue;                    // Already expanded via a cheaper entry.
        }
        closed[current] = true;

        const int col = current % m_width;
        const int row = current / m_width;

        for (int i = 0; i < 8; ++i) {
            const int nc = col + DC[i];
            const int nr = row + DR[i];
            if (!walkable(nc, nr)) {
                continue;
            }
            // A diagonal move is allowed only when both orthogonally adjacent
            // cells are also walkable, so the path never clips a wall corner.
            if (DC[i] != 0 && DR[i] != 0 &&
                (!walkable(col + DC[i], row) || !walkable(col, row + DR[i]))) {
                continue;
            }

            const int   neighbour = indexOf(nc, nr);
            if (closed[neighbour]) {
                continue;
            }
            const float stepCost  = (DC[i] != 0 && DR[i] != 0) ? std::sqrt(2.0f) : 1.0f;
            const float tentative = gScore[current] + stepCost;
            if (tentative < gScore[neighbour]) {
                gScore[neighbour]   = tentative;
                cameFrom[neighbour] = current;
                const float h = octile(goalCol - nc, goalRow - nr);
                open.push({tentative + h, neighbour});
            }
        }
    }

    // Unreachable: the goal was never given a cost.
    if (!std::isfinite(gScore[goalIndex])) {
        return {};
    }

    // Walk the parent links back from the goal, then reverse to get start->goal.
    std::vector<Vector2D> path;
    for (int at = goalIndex; at != -1; at = cameFrom[at]) {
        const int col = at % m_width;
        const int row = at / m_width;
        path.push_back({(static_cast<float>(col) + 0.5f) * TILE_SIZE,
                        (static_cast<float>(row) + 0.5f) * TILE_SIZE});
    }
    std::reverse(path.begin(), path.end());
    return path;
}
