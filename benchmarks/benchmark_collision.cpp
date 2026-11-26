#include "Collision.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

struct Pair { float ax, ay, bx, by, radius; };
struct Result { double ms; std::uint64_t hits; };

template<class Predicate>
Result measure(const std::vector<Pair>& pairs, Predicate predicate) {
    const auto start = std::chrono::steady_clock::now();
    std::uint64_t hits = 0;
    for (const auto& p : pairs) hits += predicate(p);
    const auto end = std::chrono::steady_clock::now();
    return {std::chrono::duration<double, std::milli>(end-start).count(), hits};
}

int main() {
    constexpr std::size_t count = 1000000;
    constexpr int repetitions = 11;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> position(0.f, 1100.f), radius(18.f, 48.f);
    std::vector<Pair> pairs;
    pairs.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
        pairs.push_back({position(rng), position(rng), position(rng), position(rng), radius(rng)});
    const auto baseline = [](const Pair& p) {
        const float dx = p.ax-p.bx, dy = p.ay-p.by;
        return std::sqrt(dx*dx+dy*dy) < p.radius;
    };
    const auto optimized = [](const Pair& p) {
        return collision::overlaps(p.ax,p.ay,p.bx,p.by,p.radius);
    };
    std::size_t mismatches = 0;
    for (const auto& p : pairs) mismatches += baseline(p) != optimized(p);
    if (mismatches) { std::cerr << "Mismatches: " << mismatches << '\n'; return 1; }
    auto warm = measure(pairs, baseline);
    auto warm2 = measure(pairs, optimized);
    if (warm.hits != warm2.hits) return 1;
    std::vector<double> oldTimes, newTimes;
    for (int i=0; i<repetitions; ++i) {
        Result a{}, b{};
        if (i%2==0) { a=measure(pairs, baseline); b=measure(pairs, optimized); }
        else { b=measure(pairs, optimized); a=measure(pairs, baseline); }
        if (a.hits != warm.hits || b.hits != warm.hits) return 1;
        oldTimes.push_back(a.ms); newTimes.push_back(b.ms);
    }
    std::sort(oldTimes.begin(),oldTimes.end()); std::sort(newTimes.begin(),newTimes.end());
    const double a=oldTimes[repetitions/2], b=newTimes[repetitions/2];
    std::cout << std::fixed << std::setprecision(3)
              << "Pairs: " << count << " | Repetitions: " << repetitions << '\n'
              << "Matching results: " << count << " | Collisions: " << warm.hits << '\n'
              << "sqrt median: " << a << " ms\nSquared distance median: " << b << " ms\n"
              << "Speedup: " << a/b << "x\n";
}
