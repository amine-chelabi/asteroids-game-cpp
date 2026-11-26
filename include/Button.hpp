#pragma once
#include "Common.hpp"
#include "Graphics.hpp"

class Button
{
public:
    sf::RectangleShape shape;
    std::string label;

    Button(
        float x,
        float y,
        const std::string& title
    )
    {
        shape.setSize({320.f, 56.f});
        shape.setOrigin(160.f, 28.f);
        shape.setPosition(x, y);
        shape.setOutlineThickness(1.f);

        label = title;
    }

    bool contains(sf::Vector2f mouse) const
    {
        return shape.getGlobalBounds().contains(mouse);
    }

    void draw(
        sf::RenderWindow& window,
        const sf::Font& font,
        sf::Vector2f mouse
    )
    {
        bool hovered = contains(mouse);

        shape.setFillColor(
            hovered
            ? sf::Color(20, 65, 95)
            : sf::Color(12, 25, 45)
        );

        shape.setOutlineColor(
            hovered
            ? CYAN
            : sf::Color(55, 90, 125)
        );

        if (hovered)
        {
            glow(
                window,
                shape.getPosition(),
                70.f,
                BLUE
            );
        }

        window.draw(shape);

        drawText(
            window,
            font,
            label,
            20,
            shape.getPosition().x,
            shape.getPosition().y,
            hovered ? CYAN : WHITE,
            true
        );
    }
};
