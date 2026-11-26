#pragma once
#include "Common.hpp"
#include "Physics.hpp"

class Asteroid
{
private:
    sf::ConvexShape rock;

    std::vector<sf::CircleShape> craters;

public:
    sf::Vector2f position;

    float radius;
    float speed;
    float rotation = 0;
    float rotationSpeed;
    float drift;

    Asteroid(int level)
    {
        radius = randomFloat(18.f, 48.f);

        position = {
            randomFloat(60.f, WIDTH - 60.f),
            -radius
        };

        speed = randomFloat(65.f, 125.f)
              + level * 14.f;

        drift = randomFloat(-25.f, 25.f);
        rotationSpeed = randomFloat(-65.f, 65.f);

        constexpr int POINTS = 12;

        rock.setPointCount(POINTS);

        for (int i = 0; i < POINTS; ++i)
        {
            float angle = 2.f * PI * i / POINTS;

            float r = radius
                    * randomFloat(0.76f, 1.f);

            rock.setPoint(i, {
                std::cos(angle) * r,
                std::sin(angle) * r
            });
        }

        rock.setFillColor(
            sf::Color(75, 85, 105)
        );

        rock.setOutlineColor(
            sf::Color(150, 165, 185)
        );

        rock.setOutlineThickness(1.5f);

        for (int i = 0; i < 3; ++i)
        {
            float size = radius
                       * randomFloat(0.12f, 0.22f);

            sf::CircleShape crater(size, 10);

            crater.setOrigin(size, size);

            crater.setPosition(
                randomFloat(-radius / 3, radius / 3),
                randomFloat(-radius / 3, radius / 3)
            );

            crater.setFillColor(
                sf::Color(35, 44, 65)
            );

            crater.setOutlineThickness(1.f);

            crater.setOutlineColor(
                sf::Color(100, 115, 135)
            );

            craters.push_back(crater);
        }
    }

    void update(float dt)
    {
        physics::moveAsteroid(position.x, position.y, rotation, drift,
                              speed, rotationSpeed, radius, WIDTH, dt);
    }

    void draw(sf::RenderWindow& window) const
    {
        sf::Transform transform;

        transform.translate(position);
        transform.rotate(rotation);

        sf::RenderStates states(transform);

        window.draw(rock, states);

        for (const auto& crater : craters)
            window.draw(crater, states);
    }
};
