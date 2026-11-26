#pragma once
#include <algorithm>
#include <cmath>

namespace physics {
struct Input { float x = 0.f, y = 0.f; };
inline void moveShip(float& x, float& y, Input input, float dt,
                     float width, float height) noexcept {
    const float length = std::hypot(input.x, input.y);
    if (length > 1.f) { input.x /= length; input.y /= length; }
    x = std::clamp(x + input.x * 360.f * dt, 55.f, width - 55.f);
    y = std::clamp(y + input.y * 360.f * dt, 135.f, height - 65.f);
}
inline void tickCooldown(float& value, float dt) noexcept {
    value = std::max(0.f, value - dt);
}
inline bool damage(int& lives, int& energy, float& invulnerability) noexcept {
    if (invulnerability > 0.f || lives <= 0) return false;
    invulnerability = 1.5f;
    if (--energy <= 0) { --lives; energy = lives > 0 ? 3 : 0; }
    return true;
}
inline void moveAsteroid(float& x, float& y, float& rotation, float& drift,
                         float speed, float rotationSpeed, float radius,
                         float width, float dt) noexcept {
    y += speed * dt; x += drift * dt; rotation += rotationSpeed * dt;
    if (x < radius) { x = radius; drift = std::abs(drift); }
    else if (x > width - radius) { x = width - radius; drift = -std::abs(drift); }
}
inline void moveLaser(float& y, float dt) noexcept { y -= 850.f * dt; }
}
