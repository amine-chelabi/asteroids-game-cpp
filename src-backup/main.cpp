
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

constexpr unsigned WIDTH = 1100;
constexpr unsigned HEIGHT = 760;
constexpr float PI = 3.14159265f;

const sf::Color CYAN(60, 220, 255);
const sf::Color BLUE(40, 100, 220);
const sf::Color WHITE(235, 245, 255);
const sf::Color DARK(5, 10, 25);
const sf::Color GRAY(125, 150, 180);
const sf::Color GOLD(255, 195, 70);
const sf::Color RED(255, 75, 100);

std::mt19937 rng(std::random_device{}());

float randomFloat(float min, float max)
{
    return std::uniform_real_distribution<float>(min, max)(rng);
}

float distance(sf::Vector2f a, sf::Vector2f b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

// =============================================
// GRAPHICAL UTILITIES
// =============================================

void drawText(
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

void polygon(
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

void glow(
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

// =============================================
// SPACE BACKGROUND
// =============================================

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

// =============================================
// SPACESHIP
// =============================================

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

    void update(float dt)
    {
        sf::Vector2f direction(0, 0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            direction.x -= 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            direction.x += 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            direction.y -= 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            direction.y += 1;

        float length = std::sqrt(
            direction.x * direction.x
            + direction.y * direction.y
        );

        if (length > 0)
            direction /= length;

        position += direction * 360.f * dt;

        position.x = std::clamp(
            position.x, 55.f, WIDTH - 55.f
        );

        position.y = std::clamp(
            position.y, 135.f, HEIGHT - 65.f
        );

        invulnerability = std::max(
            0.f, invulnerability - dt
        );

        fireCooldown = std::max(
            0.f, fireCooldown - dt
        );
    }

    bool damage()
    {
        if (invulnerability > 0)
            return false;

        invulnerability = 1.5f;
        --energy;

        if (energy <= 0)
        {
            --lives;
            energy = lives > 0 ? 3 : 0;
        }

        return true;
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

// =============================================
// LASER PROJECTILES
// =============================================

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

// =============================================
// ASTEROIDS
// =============================================

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
        position.y += speed * dt;
        position.x += drift * dt;

        rotation += rotationSpeed * dt;

        if (position.x < radius
            || position.x > WIDTH - radius)
            drift *= -1;
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

// =============================================
// PARTICLE EFFECTS
// =============================================

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

// =============================================
// HIGH SCORES
// =============================================

class HighScores
{
public:
    std::vector<int> load() const
    {
        std::ifstream file("point.txt");

        std::vector<int> scores;
        int value;

        while (file >> value)
        {
            if (value >= 0)
                scores.push_back(value);
        }

        std::sort(
            scores.rbegin(),
            scores.rend()
        );

        if (scores.size() > 10)
            scores.resize(10);

        return scores;
    }

    void save(int score) const
    {
        auto scores = load();

        scores.push_back(score);

        std::sort(
            scores.rbegin(),
            scores.rend()
        );

        if (scores.size() > 10)
            scores.resize(10);

        std::ofstream file("point.txt");

        for (int value : scores)
            file << value << '\n';
    }
};

// =============================================
// MENU BUTTON
// =============================================

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

// =============================================
// GAME
// =============================================

enum class State
{
    Menu,
    Playing,
    Paused,
    Scores,
    Help,
    GameOver,
    Victory
};

class Game
{
private:
    sf::RenderWindow window;

    sf::Font font;

    SpaceBackground background;

    Spaceship ship;

    Particles particles;

    HighScores highScores;

    std::vector<Laser> lasers;

    std::vector<Asteroid> asteroids;

    State state = State::Menu;

    int score = 0;
    int level = 1;

    float spawnTimer = 0;
    float elapsed = 0;

    Button playButton{
        WIDTH / 2.f, 405, "NEW GAME"
    };

    Button scoresButton{
        WIDTH / 2.f, 475, "HIGH SCORES"
    };

    Button helpButton{
        WIDTH / 2.f, 545, "HOW TO PLAY"
    };

    Button exitButton{
        WIDTH / 2.f, 615, "EXIT GAME"
    };

public:
    Game()
        : window(
            sf::VideoMode(WIDTH, HEIGHT),
            "ASTEROIDS - Space Survival",
            sf::Style::Titlebar | sf::Style::Close
        )
    {
        window.setFramerateLimit(60);

        bool loaded =
            font.loadFromFile("assets/font.ttf");

        if (!loaded)
            loaded = font.loadFromFile(
                "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf"
            );

        if (!loaded)
            loaded = font.loadFromFile(
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
            );

        if (!loaded)
        {
            std::cerr << "Font not found.\n";
            std::cerr << "Copy a font to font.ttf\n";

            window.close();
        }
    }

    void startGame()
    {
        ship = Spaceship();

        lasers.clear();
        asteroids.clear();
        particles.clear();

        score = 0;
        level = 1;

        spawnTimer = 0;

        state = State::Playing;
    }

    void handleEvents()
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                auto key = event.key.code;

                if (state == State::Menu)
                {
                    if (key == sf::Keyboard::Enter
                        || key == sf::Keyboard::J)
                        startGame();

                    else if (key == sf::Keyboard::S)
                        state = State::Scores;

                    else if (key == sf::Keyboard::H)
                        state = State::Help;

                    else if (key == sf::Keyboard::Escape)
                        window.close();
                }
                else if (state == State::Playing)
                {
                    if (key == sf::Keyboard::P
                        || key == sf::Keyboard::Escape)
                        state = State::Paused;

                    else if (key == sf::Keyboard::Q)
                        state = State::Menu;
                }
                else if (state == State::Paused)
                {
                    if (key == sf::Keyboard::P
                        || key == sf::Keyboard::Escape)
                        state = State::Playing;

                    else if (key == sf::Keyboard::Q)
                        state = State::Menu;
                }
                else
                {
                    if (key == sf::Keyboard::Enter
                        || key == sf::Keyboard::Escape)
                        state = State::Menu;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed
                && event.mouseButton.button == sf::Mouse::Left
                && state == State::Menu)
            {
                sf::Vector2f mouse =
                    window.mapPixelToCoords({
                        event.mouseButton.x,
                        event.mouseButton.y
                    });

                if (playButton.contains(mouse))
                    startGame();

                else if (scoresButton.contains(mouse))
                    state = State::Scores;

                else if (helpButton.contains(mouse))
                    state = State::Help;

                else if (exitButton.contains(mouse))
                    window.close();
            }
        }
    }

    void updateGame(float dt)
    {
        ship.update(dt);

        level = 1 + score / 30;

        // Dual laser cannons

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::B))
            && ship.fireCooldown <= 0)
        {
            lasers.push_back({
                ship.position + sf::Vector2f(-25, -5)
            });

            lasers.push_back({
                ship.position + sf::Vector2f(25, -5)
            });

            ship.fireCooldown = 0.18f;
        }

        // Asteroid generation

        spawnTimer += dt;

        float interval = std::max(
            0.25f,
            1.05f - level * 0.10f
        );

        if (spawnTimer >= interval)
        {
            asteroids.emplace_back(level);
            spawnTimer = 0;
        }

        for (auto& laser : lasers)
            laser.update(dt);

        for (auto& asteroid : asteroids)
            asteroid.update(dt);

        // Laser / asteroid collisions

        for (std::size_t i = 0; i < asteroids.size();)
        {
            bool destroyed = false;

            for (std::size_t j = 0; j < lasers.size(); ++j)
            {
                if (distance(
                    asteroids[i].position,
                    lasers[j].position
                    ) < asteroids[i].radius)
                {
                    particles.explode(
                        asteroids[i].position,
                        GOLD,
                        35
                    );

                    particles.explode(
                        asteroids[i].position,
                        RED,
                        12
                    );

                    lasers.erase(lasers.begin() + j);

                    asteroids.erase(
                        asteroids.begin() + i
                    );

                    score += 10;
                    destroyed = true;

                    break;
                }
            }

            if (!destroyed)
                ++i;
        }

        // Ship / asteroid collisions

        for (std::size_t i = 0; i < asteroids.size();)
        {
            if (distance(
                asteroids[i].position,
                ship.position
                ) < asteroids[i].radius + 23.f)
            {
                if (ship.damage())
                {
                    particles.explode(
                        ship.position,
                        CYAN,
                        40
                    );
                }

                asteroids.erase(
                    asteroids.begin() + i
                );
            }
            else
                ++i;
        }

        lasers.erase(
            std::remove_if(
                lasers.begin(),
                lasers.end(),
                [](const Laser& laser)
                {
                    return laser.position.y < 90;
                }
            ),
            lasers.end()
        );

        asteroids.erase(
            std::remove_if(
                asteroids.begin(),
                asteroids.end(),
                [](const Asteroid& asteroid)
                {
                    return asteroid.position.y
                        > HEIGHT + asteroid.radius;
                }
            ),
            asteroids.end()
        );

        particles.update(dt);

        if (ship.lives <= 0)
        {
            highScores.save(score);
            state = State::GameOver;
        }
        else if (score >= 100)
        {
            highScores.save(score);
            state = State::Victory;
        }
    }

    void drawMenu()
    {
        drawText(
            window, font,
            "A S T E R O I D S",
            63,
            WIDTH / 2.f, 95,
            CYAN, true
        );

        drawText(
            window, font,
            "S P A C E   S U R V I V A L",
            20,
            WIDTH / 2.f, 158,
            GRAY, true
        );

        Spaceship preview;

        preview.position = {
            WIDTH / 2.f, 270
        };

        preview.draw(window, elapsed);

        sf::Vector2f mouse =
            window.mapPixelToCoords(
                sf::Mouse::getPosition(window)
            );

        playButton.draw(window, font, mouse);
        scoresButton.draw(window, font, mouse);
        helpButton.draw(window, font, mouse);
        exitButton.draw(window, font, mouse);

        drawText(
            window, font,
            "DEVELOPED BY AMINE CHELABI",
            15,
            WIDTH / 2.f, 705,
            GRAY, true
        );

        drawText(
            window, font,
            "C++17  /  SFML",
            12,
            WIDTH / 2.f, 730,
            GRAY, true
        );
    }

    void drawHUD()
    {
        sf::RectangleShape panel({
            static_cast<float>(WIDTH), 90.f
        });

        panel.setFillColor(
            sf::Color(7, 15, 32, 245)
        );

        window.draw(panel);

        sf::RectangleShape line({
            static_cast<float>(WIDTH), 2.f
        });

        line.setPosition(0, 88);
        line.setFillColor(sf::Color(35, 90, 125));

        window.draw(line);

        drawText(window, font, "LIVES", 14, 30, 13, GRAY);

        for (int i = 0; i < 3; ++i)
        {
            sf::RectangleShape bar({40, 12});

            bar.setPosition(30 + i * 48, 42);

            bar.setFillColor(
                i < ship.lives
                ? RED
                : sf::Color(40, 50, 70)
            );

            window.draw(bar);
        }

        drawText(
            window, font,
            "SHIELD ENERGY",
            14, 225, 13, GRAY
        );

        for (int i = 0; i < 3; ++i)
        {
            sf::RectangleShape bar({47, 12});

            bar.setPosition(225 + i * 53, 42);

            bar.setFillColor(
                i < ship.energy
                ? CYAN
                : sf::Color(40, 50, 70)
            );

            window.draw(bar);
        }

        drawText(
            window, font,
            "WAVE",
            14, 550, 13, GRAY
        );

        drawText(
            window, font,
            std::to_string(level),
            30, 550, 34, WHITE
        );

        drawText(
            window, font,
            "MISSION PROGRESS",
            13, 700, 13, GRAY
        );

        sf::RectangleShape progress({140, 10});

        progress.setPosition(700, 45);

        progress.setFillColor(
            sf::Color(40, 50, 70)
        );

        window.draw(progress);

        progress.setSize({
            140.f * std::min(score / 100.f, 1.f),
            10.f
        });

        progress.setFillColor(CYAN);

        window.draw(progress);

        std::ostringstream value;

        value << std::setw(5)
              << std::setfill('0')
              << score;

        drawText(
            window, font,
            "SCORE",
            14, 925, 13, GRAY
        );

        drawText(
            window, font,
            value.str(),
            30, 925, 34, GOLD
        );

        drawText(
            window, font,
            "ARROWS / WASD : MOVE    |    SPACE : FIRE    |    P : PAUSE",
            13,
            WIDTH / 2.f, HEIGHT - 16.f,
            GRAY, true
        );
    }

    void drawGame()
    {
        for (const auto& asteroid : asteroids)
            asteroid.draw(window);

        for (const auto& laser : lasers)
            laser.draw(window);

        particles.draw(window);

        ship.draw(window, elapsed);

        drawHUD();
    }

    void drawOverlay(
        const std::string& title,
        const std::string& subtitle,
        sf::Color color
    )
    {
        sf::RectangleShape shade({
            static_cast<float>(WIDTH),
            static_cast<float>(HEIGHT)
        });

        shade.setFillColor(
            sf::Color(3, 7, 18, 200)
        );

        window.draw(shade);

        sf::RectangleShape panel({650, 250});

        panel.setOrigin(325, 125);

        panel.setPosition(
            WIDTH / 2.f,
            HEIGHT / 2.f
        );

        panel.setFillColor(
            sf::Color(10, 22, 42)
        );

        panel.setOutlineThickness(1.5f);
        panel.setOutlineColor(color);

        window.draw(panel);

        drawText(
            window, font,
            title,
            42,
            WIDTH / 2.f,
            HEIGHT / 2.f - 65,
            color, true
        );

        drawText(
            window, font,
            subtitle,
            19,
            WIDTH / 2.f,
            HEIGHT / 2.f + 5,
            WHITE, true
        );

        drawText(
            window, font,
            state == State::Paused
            ? "P : Resume    |    Q : Main menu"
            : "Press ENTER to return to menu",
            16,
            WIDTH / 2.f,
            HEIGHT / 2.f + 70,
            GRAY, true
        );
    }

    void drawScores()
    {
        drawText(
            window, font,
            "HIGH SCORES",
            50, WIDTH / 2.f, 90,
            GOLD, true
        );

        auto scores = highScores.load();

        if (scores.empty())
        {
            drawText(
                window, font,
                "No scores recorded yet",
                24, WIDTH / 2.f, 340,
                GRAY, true
            );
        }

        for (std::size_t i = 0; i < scores.size(); ++i)
        {
            float y = 170.f + i * 46.f;

            sf::RectangleShape row({460, 38});

            row.setOrigin(230, 19);
            row.setPosition(WIDTH / 2.f, y);

            row.setFillColor(
                sf::Color(20, 35, 60, 220)
            );

            window.draw(row);

            drawText(
                window, font,
                "#" + std::to_string(i + 1),
                18, 355, y,
                GRAY, true
            );

            drawText(
                window, font,
                std::to_string(scores[i]),
                22, 720, y,
                i == 0 ? GOLD : WHITE,
                true
            );
        }

        drawText(
            window, font,
            "Press ENTER to return",
            18, WIDTH / 2.f, 710,
            CYAN, true
        );
    }

    void drawHelp()
    {
        drawText(
            window, font,
            "MISSION BRIEFING",
            46, WIDTH / 2.f, 110,
            CYAN, true
        );

        std::vector<std::string> instructions = {
            "Destroy incoming asteroids to survive.",
            "Arrow keys / WASD : Move spaceship",
            "SPACE or B : Fire laser cannons",
            "P : Pause the mission",
            "Q : Return to main menu",
            "Three lives and three shield points per life",
            "Destroy an asteroid to earn 10 points",
            "Reach 100 points to complete your mission"
        };

        for (std::size_t i = 0; i < instructions.size(); ++i)
        {
            drawText(
                window, font,
                instructions[i],
                21, WIDTH / 2.f,
                205.f + i * 48.f,
                WHITE, true
            );
        }

        drawText(
            window, font,
            "Press ENTER to return",
            18, WIDTH / 2.f, 710,
            CYAN, true
        );
    }

    void run()
    {
        sf::Clock clock;

        while (window.isOpen())
        {
            float dt = std::min(
                clock.restart().asSeconds(),
                0.05f
            );

            elapsed += dt;

            handleEvents();

            if (!window.isOpen())
                break;

            background.update(dt);

            if (state == State::Playing)
                updateGame(dt);

            window.clear(DARK);

            background.draw(window);

            switch (state)
            {
                case State::Menu:
                    drawMenu();
                    break;

                case State::Playing:
                    drawGame();
                    break;

                case State::Paused:
                    drawGame();

                    drawOverlay(
                        "PAUSED",
                        "Mission suspended",
                        CYAN
                    );

                    break;

                case State::Scores:
                    drawScores();
                    break;

                case State::Help:
                    drawHelp();
                    break;

                case State::GameOver:
                    drawOverlay(
                        "GAME OVER",
                        "Final score: " + std::to_string(score),
                        RED
                    );

                    break;

                case State::Victory:
                    drawOverlay(
                        "MISSION COMPLETE",
                        "Final score: " + std::to_string(score),
                        GOLD
                    );

                    break;
            }

            window.display();
        }
    }
};

int main()
{
    Game game;
    game.run();

    return 0;
}

