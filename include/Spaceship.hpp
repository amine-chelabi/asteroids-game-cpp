#pragma once
#include "Common.hpp"
#include "Graphics.hpp"
#include "Physics.hpp"

class Spaceship
{
public:
    sf::Vector2f position{
        WIDTH / 2.f,
        HEIGHT - 135.f
    };

    int lives = 3;
    int energy = 3;

    float invulnerability = 0.f;
    float fireCooldown = 0.f;

    void update(float dt, physics::Input input)
    {
        physics::moveShip(position.x, position.y, input, dt, WIDTH, HEIGHT);
        physics::tickCooldown(invulnerability, dt);
        physics::tickCooldown(fireCooldown, dt);
    }

    bool damage()
    {
        return physics::damage(lives, energy, invulnerability);
    }

    void draw(
        sf::RenderWindow& window,
        float time
    ) const
    {
        if (invulnerability > 0
            && static_cast<int>(time * 12) % 2 == 0)
            return;

        float flame = 35.f + std::sin(time * 30.f) * 9.f;

        // Engine glow

        glow(
            window,
            position + sf::Vector2f(0, 24),
            55,
            CYAN
        );

        // Thrusters

        polygon(window, position,
        {
            {-17, 26}, {-12, flame + 25},
            {-6, 26}
        }, BLUE);

        polygon(window, position,
        {
            {6, 26}, {12, flame + 25},
            {17, 26}
        }, BLUE);

        polygon(window, position,
        {
            {-5, 27}, {0, flame + 15},
            {5, 27}
        }, CYAN);

        // Main wings

        polygon(window, position,
        {
            {0, -48},
            {-18, -12},
            {-48, 28},
            {-42, 34},
            {-13, 18},
            {0, 32},
            {13, 18},
            {42, 34},
            {48, 28},
            {18, -12}
        },
        sf::Color(150, 175, 205),
        WHITE,
        1.4f);

        // Left wing armor

        polygon(window, position,
        {
            {-18, -5},
            {-43, 26},
            {-14, 13}
        },
        sf::Color(45, 70, 105),
        sf::Color(100, 150, 200),
        1.f);

        // Right wing armor

        polygon(window, position,
        {
            {18, -5},
            {43, 26},
            {14, 13}
        },
        sf::Color(45, 70, 105),
        sf::Color(100, 150, 200),
        1.f);

        // Central body

        polygon(window, position,
        {
            {0, -43},
            {-11, -7},
            {-10, 23},
            {0, 30},
            {10, 23},
            {11, -7}
        },
        sf::Color(215, 230, 245),
        sf::Color(90, 130, 180),
        1.f);

        // Cockpit

        polygon(window, position,
        {
            {0, -26},
            {-7, -5},
            {0, 9},
            {7, -5}
        },
        sf::Color(10, 55, 100),
        CYAN,
        1.5f);

        polygon(window, position,
        {
            {0, -21},
            {-3, -7},
            {3, -7}
        },
        sf::Color(130, 240, 255));

        // Wing lights

        for (float x : {-35.f, 35.f})
        {
            sf::CircleShape light(3.f);
            light.setOrigin(3, 3);

            light.setPosition(
                position + sf::Vector2f(x, 24)
            );

            light.setFillColor(CYAN);
            window.draw(light);
        }
    }
};
