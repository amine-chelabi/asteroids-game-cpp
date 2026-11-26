#pragma once
#include "Common.hpp"

struct Laser
{
    sf::Vector2f position;

    void update(float dt)
    {
        position.y -= 850.f * dt;
    }

    void draw(sf::RenderWindow& window) const
    {
        sf::RectangleShape aura({10.f, 34.f});
        aura.setOrigin(5.f, 17.f);
        aura.setPosition(position);
        aura.setFillColor(sf::Color(20, 190, 255, 55));

        window.draw(aura, sf::BlendAdd);

        sf::RectangleShape beam({3.f, 28.f});
        beam.setOrigin(1.5f, 14.f);
        beam.setPosition(position);
        beam.setFillColor(WHITE);

        window.draw(beam);
    }
};
