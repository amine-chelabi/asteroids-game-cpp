#pragma once
#include "Common.hpp"

inline void drawText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& value,
    unsigned size,
    float x,
    float y,
    sf::Color color = WHITE,
    bool centered = false
)
{
    sf::Text text(value, font, size);
    text.setFillColor(color);

    if (centered)
    {
        sf::FloatRect bounds = text.getLocalBounds();

        text.setOrigin(
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f
        );
    }

    text.setPosition(x, y);
    window.draw(text);
}

inline void polygon(
    sf::RenderWindow& window,
    sf::Vector2f position,
    const std::vector<sf::Vector2f>& points,
    sf::Color fill,
    sf::Color outline = sf::Color::Transparent,
    float thickness = 0.f
)
{
    sf::ConvexShape shape;
    shape.setPointCount(points.size());

    for (std::size_t i = 0; i < points.size(); ++i)
        shape.setPoint(i, points[i]);

    shape.setPosition(position);
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    shape.setOutlineThickness(thickness);

    window.draw(shape);
}

inline void glow(
    sf::RenderWindow& window,
    sf::Vector2f position,
    float radius,
    sf::Color color
)
{
    for (int i = 5; i >= 1; --i)
    {
        float r = radius * i / 5.f;

        sf::CircleShape circle(r, 48);
        circle.setOrigin(r, r);
        circle.setPosition(position);

        color.a = static_cast<sf::Uint8>(10 + (5 - i) * 5);
        circle.setFillColor(color);

        window.draw(circle, sf::BlendAdd);
    }
}
