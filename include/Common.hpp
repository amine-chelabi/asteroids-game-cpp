#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

inline constexpr unsigned WIDTH = 1100;
inline constexpr unsigned HEIGHT = 760;
inline constexpr float PI = 3.14159265f;

inline const sf::Color CYAN(60, 220, 255);
inline const sf::Color BLUE(40, 100, 220);
inline const sf::Color WHITE(235, 245, 255);
inline const sf::Color DARK(5, 10, 25);
inline const sf::Color GRAY(125, 150, 180);
inline const sf::Color GOLD(255, 195, 70);
inline const sf::Color RED(255, 75, 100);

inline std::mt19937 rng(std::random_device{}());

inline float randomFloat(float min, float max)
{
    return std::uniform_real_distribution<float>(min, max)(rng);
}

