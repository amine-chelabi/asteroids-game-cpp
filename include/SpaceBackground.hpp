#pragma once
#include "Common.hpp"
#include "Graphics.hpp"

struct Star
{
    sf::Vector2f position;
    float speed;
    float radius;
    sf::Color color;
};

class SpaceBackground
{
private:
    std::vector<Star> stars;

public:
    SpaceBackground()
    {
        for (int i = 0; i < 250; ++i)
        {
            Star star;

            star.position = {
                randomFloat(0, WIDTH),
                randomFloat(0, HEIGHT)
            };

            star.speed = randomFloat(10, 90);
            star.radius = randomFloat(0.5f, 1.8f);

            star.color = sf::Color(
                180, 215, 255,
                static_cast<sf::Uint8>(
                    randomFloat(50, 230)
                )
            );

            stars.push_back(star);
        }
    }

    void update(float dt)
    {
        for (auto& star : stars)
        {
            star.position.y += star.speed * dt;

            if (star.position.y > HEIGHT)
            {
                star.position.y = 0;
                star.position.x = randomFloat(0, WIDTH);
            }
        }
    }

    void draw(sf::RenderWindow& window)
    {
        sf::VertexArray background(sf::Quads, 4);

        background[0] = sf::Vertex(
            {0, 0}, sf::Color(5, 10, 26)
        );

        background[1] = sf::Vertex(
            {WIDTH, 0}, sf::Color(8, 15, 35)
        );

        background[2] = sf::Vertex(
            {WIDTH, HEIGHT}, sf::Color(14, 25, 55)
        );

        background[3] = sf::Vertex(
            {0, HEIGHT}, sf::Color(4, 9, 20)
        );

        window.draw(background);

        glow(
            window,
            {900, 200},
            200,
            sf::Color(20, 45, 130)
        );

        glow(
            window,
            {140, 540},
            180,
            sf::Color(30, 20, 90)
        );

        for (const auto& star : stars)
        {
            sf::CircleShape point(star.radius);

            point.setPosition(star.position);
            point.setFillColor(star.color);

            window.draw(point);
        }
    }
};
