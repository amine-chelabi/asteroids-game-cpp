#pragma once
#include "Common.hpp"
#include "Graphics.hpp"
#include "SpaceBackground.hpp"
#include "Spaceship.hpp"
#include "Laser.hpp"
#include "Asteroid.hpp"
#include "Particles.hpp"
#include "HighScores.hpp"
#include "Button.hpp"
#include "Collision.hpp"

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
    std::vector<int> cachedScores;

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
                        { cachedScores = highScores.load(); state = State::Scores; }

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
                    { cachedScores = highScores.load(); state = State::Scores; }

                else if (helpButton.contains(mouse))
                    state = State::Help;

                else if (exitButton.contains(mouse))
                    window.close();
            }
        }
    }

    void updateGame(float dt)
    {
        physics::Input input;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) input.x -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) input.x += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) input.y -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) input.y += 1;
        ship.update(dt, input);

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
                if (collision::overlaps(asteroids[i].position.x, asteroids[i].position.y,
                    lasers[j].position.x, lasers[j].position.y, asteroids[i].radius))
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
            if (collision::overlaps(asteroids[i].position.x, asteroids[i].position.y,
                ship.position.x, ship.position.y, asteroids[i].radius + 23.f))
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

        const auto& scores = cachedScores;

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


