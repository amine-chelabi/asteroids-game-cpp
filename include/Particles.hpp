#pragma once
#include "Common.hpp"

struct Particle
{
    sf::Vector2f position;
    sf::Vector2f velocity;

    float life;
    float size;

    sf::Color color;
};

class Particles
{
private:
    std::vector<Particle> particles;

public:
    void explode(
        sf::Vector2f position,
        sf::Color color,
        int amount = 30
    )
    {
        for (int i = 0; i < amount; ++i)
        {
            float angle = randomFloat(0, 2 * PI);
            float speed = randomFloat(45, 250);

            particles.push_back({
                position,
                {
                    std::cos(angle) * speed,
                    std::sin(angle) * speed
                },
                randomFloat(0.3f, 0.9f),
                randomFloat(1.5f, 5.f),
                color
            });
        }
    }

    void update(float dt)
    {
        for (auto& particle : particles)
        {
            particle.position += particle.velocity * dt;

            particle.velocity *=
                std::max(0.f, 1.f - dt * 2.f);

            particle.life -= dt;
        }

        particles.erase(
            std::remove_if(
                particles.begin(),
                particles.end(),
                [](const Particle& p)
                {
                    return p.life <= 0;
                }
            ),
            particles.end()
        );
    }

    void draw(sf::RenderWindow& window) const
    {
        for (const auto& particle : particles)
        {
            sf::CircleShape point(particle.size);

            point.setOrigin(
                particle.size,
                particle.size
            );

            point.setPosition(particle.position);

            sf::Color color = particle.color;

            color.a = static_cast<sf::Uint8>(
                std::clamp(
                    particle.life * 300.f,
                    0.f, 255.f
                )
            );

            point.setFillColor(color);

            window.draw(point, sf::BlendAdd);
        }
    }

    void clear()
    {
        particles.clear();
    }
};
