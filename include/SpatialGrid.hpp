#pragma once
#include "Collision.hpp"
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>
namespace spatial {
struct Point { float x,y; };
struct Circle { float x,y,radius; };
struct Match { std::size_t asteroid, laser; bool operator==(const Match& b) const { return asteroid==b.asteroid && laser==b.laser; } };
struct Result { std::vector<Match> matches; std::size_t comparisons=0; };
inline Result match(const std::vector<Circle>& circles, const std::vector<Point>& points, bool grid=true) {
    constexpr float cell=64.f;
    auto index=[](float x){ return static_cast<int>(std::floor(x/cell)); };
    auto key=[](int x,int y){return (std::uint64_t(static_cast<std::uint32_t>(x))<<32) | static_cast<std::uint32_t>(y);};
    std::unordered_map<std::uint64_t,std::vector<std::size_t>> buckets;
    if(grid) { buckets.reserve(points.size()); for(std::size_t j=0;j<points.size();++j) buckets[key(index(points[j].x),index(points[j].y))].push_back(j); }
    std::vector<bool> used(points.size(),false); Result out;
    for(std::size_t i=0;i<circles.size();++i) {
        const auto& c=circles[i]; if(c.radius<=0) continue;
        auto best=points.size();
        auto check=[&](std::size_t j){ if(used[j] || j>=best) return; ++out.comparisons; if(collision::overlaps(c.x,c.y,points[j].x,points[j].y,c.radius)) best=j; };
        if(grid) { for(int y=index(c.y-c.radius);y<=index(c.y+c.radius);++y) for(int x=index(c.x-c.radius);x<=index(c.x+c.radius);++x) { auto it=buckets.find(key(x,y)); if(it!=buckets.end()) for(auto j:it->second) check(j); } }
        else { for(std::size_t j=0;j<points.size() && best==points.size();++j) check(j); }
        if(best!=points.size()){used[best]=true;out.matches.push_back({i,best});}
    } return out;
}
}
