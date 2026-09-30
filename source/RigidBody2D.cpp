#include "RigidBody2D.hpp"

#include <cassert>

void RigidBody2D::integrate(float dt) noexcept {
    assert(dt > 0.0f && "integrate(): dt must be positive");

    // Semi-implicit (symplectic) Euler: velocity is updated first, then
    // position uses the NEW velocity, which is more stable than plain
    // (explicit) Euler for interactive simulation.
    velocity += acceleration * dt;
    position += velocity * dt;
}
