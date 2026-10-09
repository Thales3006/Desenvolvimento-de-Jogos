#include "Dungeon2D.hpp"

#include <algorithm>
#include <cassert>
#include <random>

namespace {

constexpr int ROOM_MIN   = 5;   ///< Smallest room side, in tiles.
constexpr int CORRIDOR_W = 3;   ///< Corridor width, in tiles.
/// A leaf region must hold a ROOM_MIN room plus a one-tile margin on each side,
/// so a region is only split when both halves stay at least this big.
constexpr int LEAF_MIN   = ROOM_MIN + 2;   // 7
/// Cap on the tree depth, to keep rooms from degenerating to the minimum size.
constexpr int MAX_DEPTH  = 5;

int rectWidth (const Rect& r) { return r.right  - r.left + 1; }
int rectHeight(const Rect& r) { return r.bottom - r.top  + 1; }

/// Splits a node's region into two children. Returns false if too small.
bool splitNode(BSPNode& node, std::mt19937& rng) {
    const int w = rectWidth(node.region);
    const int h = rectHeight(node.region);

    const bool canSplitV = w >= 2 * LEAF_MIN;   // a vertical cut (left | right)
    const bool canSplitH = h >= 2 * LEAF_MIN;   // a horizontal cut (top / bottom)
    if (!canSplitV && !canSplitH) return false;

    // Prefer cutting the longer side so regions stay roughly square.
    bool splitVertically;
    if (canSplitV && canSplitH) {
        if      (w > h * 5 / 4) splitVertically = true;
        else if (h > w * 5 / 4) splitVertically = false;
        else                    splitVertically = (rng() & 1u) != 0;
    } else {
        splitVertically = canSplitV;
    }

    node.left  = std::make_unique<BSPNode>();
    node.right = std::make_unique<BSPNode>();

    if (splitVertically) {
        std::uniform_int_distribution<int> at(node.region.left + LEAF_MIN - 1,
                                              node.region.right - LEAF_MIN);
        const int x = at(rng);
        node.left->region  = {node.region.left, node.region.top, x, node.region.bottom};
        node.right->region = {x + 1, node.region.top, node.region.right, node.region.bottom};
    } else {
        std::uniform_int_distribution<int> at(node.region.top + LEAF_MIN - 1,
                                              node.region.bottom - LEAF_MIN);
        const int y = at(rng);
        node.left->region  = {node.region.left, node.region.top, node.region.right, y};
        node.right->region = {node.region.left, y + 1, node.region.right, node.region.bottom};
    }
    return true;
}

void buildTree(BSPNode& node, std::mt19937& rng, int depth) {
    if (depth >= MAX_DEPTH)      return;
    if (!splitNode(node, rng))   return;
    buildTree(*node.left,  rng, depth + 1);
    buildTree(*node.right, rng, depth + 1);
}

/// Carves a room inside every leaf's region, keeping a one-tile margin.
void carveRooms(BSPNode& node, std::mt19937& rng) {
    if (!node.isLeaf()) {
        if (node.left)  carveRooms(*node.left,  rng);
        if (node.right) carveRooms(*node.right, rng);
        return;
    }

    int x1 = node.region.left, y1 = node.region.top;
    int x2 = node.region.right, y2 = node.region.bottom;
    if (rectWidth(node.region)  > ROOM_MIN) { x1 += 1; x2 -= 1; }
    if (rectHeight(node.region) > ROOM_MIN) { y1 += 1; y2 -= 1; }

    const int availW = x2 - x1 + 1;
    const int availH = y2 - y1 + 1;
    const int roomW = (availW <= ROOM_MIN)
                    ? availW : std::uniform_int_distribution<int>(ROOM_MIN, availW)(rng);
    const int roomH = (availH <= ROOM_MIN)
                    ? availH : std::uniform_int_distribution<int>(ROOM_MIN, availH)(rng);
    const int roomX = std::uniform_int_distribution<int>(x1, x2 - roomW + 1)(rng);
    const int roomY = std::uniform_int_distribution<int>(y1, y2 - roomH + 1)(rng);

    node.room = {roomX, roomY, roomX + roomW - 1, roomY + roomH - 1};
}

/// Centre (in tiles) of a representative room reached by always descending left.
void roomCentre(const BSPNode& node, int& cx, int& cy) {
    if (node.isLeaf()) {
        cx = (node.room.left + node.room.right) / 2;
        cy = (node.room.top  + node.room.bottom) / 2;
        return;
    }
    if (node.left) roomCentre(*node.left,  cx, cy);
    else           roomCentre(*node.right, cx, cy);
}

/// Appends an L-shaped, CORRIDOR_W-wide path between two tile points.
void addCorridor(std::vector<Corridor>& corridors, int ax, int ay, int bx, int by) {
    const int half = CORRIDOR_W / 2;   // 1 for a 3-wide corridor
    // Horizontal leg along row ay, then vertical leg along column bx; the two
    // legs overlap at (bx, ay), so together they form a connected path.
    corridors.push_back({std::min(ax, bx), ay - half, std::max(ax, bx), ay + half});
    corridors.push_back({bx - half, std::min(ay, by), bx + half, std::max(ay, by)});
}

/// Connects the two children of every internal node, bottom-up, which makes
/// the whole dungeon a single connected space.
void connect(BSPNode& node, std::vector<Corridor>& corridors) {
    if (node.isLeaf()) return;
    connect(*node.left,  corridors);
    connect(*node.right, corridors);

    int ax, ay, bx, by;
    roomCentre(*node.left,  ax, ay);
    roomCentre(*node.right, bx, by);
    addCorridor(corridors, ax, ay, bx, by);
}

} // namespace

Dungeon2D::Dungeon2D(int widthTiles, int heightTiles)
    : width(widthTiles), height(heightTiles) {
    assert(widthTiles > 0 && heightTiles > 0 && "Dungeon2D: dimensions must be positive");
}

void Dungeon2D::generate(unsigned seed) {
    std::mt19937 rng(seed);
    corridors.clear();

    root = std::make_unique<BSPNode>();
    root->region = {0, 0, width - 1, height - 1};

    buildTree(*root, rng, 0);
    carveRooms(*root, rng);
    connect(*root, corridors);
}
