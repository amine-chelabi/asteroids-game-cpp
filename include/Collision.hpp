#pragma once
namespace collision {
constexpr bool overlaps(float ax, float ay, float bx, float by, float radius) noexcept {
    const float dx = ax - bx;
    const float dy = ay - by;
    return radius > 0.f && dx * dx + dy * dy < radius * radius;
}
}
