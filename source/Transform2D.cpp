#include "Transform2D.hpp"

#include <cmath>

// --- Factories -------------------------------------------------------------

Transform2D Transform2D::translation(float tx, float ty) noexcept {
    Transform2D t;  // starts as identity
    t.m[2][0] = tx;
    t.m[2][1] = ty;
    return t;
}

Transform2D Transform2D::rotation(float angle_rad) noexcept {
    const float c = std::cos(angle_rad);
    const float s = std::sin(angle_rad);
    Transform2D t;  // starts as identity
    t.m[0][0] =  c;  t.m[0][1] = s;
    t.m[1][0] = -s;  t.m[1][1] = c;
    return t;
}

Transform2D Transform2D::scale(float sx, float sy) noexcept {
    Transform2D t;  // starts as identity
    t.m[0][0] = sx;
    t.m[1][1] = sy;
    return t;
}

// --- Composition -----------------------------------------------------------

Transform2D Transform2D::operator*(const Transform2D& rhs) const noexcept {
    // Row-major product: result[i][j] = sum_k this->m[i][k] * rhs.m[k][j].
    // With row vectors, v * (A * B) == (v * A) * B, so this applies *this first.
    Transform2D result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) {
                sum += m[i][k] * rhs.m[k][j];
            }
            result.m[i][j] = sum;
        }
    }
    return result;
}

Transform2D& Transform2D::operator*=(const Transform2D& rhs) noexcept {
    *this = *this * rhs;
    return *this;
}

// --- Application -----------------------------------------------------------

Vector2D Transform2D::transform_point(const Vector2D& point) const noexcept {
    // [x, y, 1] * M : the third row (translation) IS applied.
    return {
        point.x * m[0][0] + point.y * m[1][0] + m[2][0],
        point.x * m[0][1] + point.y * m[1][1] + m[2][1]
    };
}

Vector2D Transform2D::transform_vector(const Vector2D& direction) const noexcept {
    // [x, y, 0] * M : the third row (translation) is NOT applied.
    return {
        direction.x * m[0][0] + direction.y * m[1][0],
        direction.x * m[0][1] + direction.y * m[1][1]
    };
}
