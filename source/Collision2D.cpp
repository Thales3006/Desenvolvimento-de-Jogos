#include "Collision2D.hpp"

// --- AABB --------------------------------------------------------------

bool AABB::intersects(const AABB& other) const noexcept {
    // Separating axis test: no overlap if one box is entirely to a side of
    // the other on either axis. <= / >= so touching edges still count.
    if (max.x < other.min.x || min.x > other.max.x) {
        return false;
    }
    if (max.y < other.min.y || min.y > other.max.y) {
        return false;
    }
    return true;
}

// --- Collision2D ---------------------------------------------------------

AABB Collision2D::bounds(const Vector2D& position) const noexcept {
    return {position - halfExtents, position + halfExtents};
}
