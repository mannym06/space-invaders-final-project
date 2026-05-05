#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

/*
    Space Invaders - C++ SFML Final Project
    ---------------------------------------

    Team Members:
    - Nabila
    - Justin
    - Denim
    - Manuel

    Team responsibility map:
    - Manuel: Game constants, collision detection, main game loop,
      bullet cleanup, scoring, lives, win/loss logic, and game state.
    - Nabila: Enemy data, shield data, game reset, level setup,
      enemy grid creation, and shield block creation.
    - Justin: Player data, keyboard event handling, player movement,
      player shooting, enemy movement, and enemy shooting.
    - Denim: Rendering, colors, star background, player/enemy drawing,
      bullet drawing, shield drawing, HUD/life indicators, and visual layout.

    Note: Keep these names only if they accurately match your team's work.
    The course requirements ask for notation showing who wrote each part.

    Program summary:
    This program creates a Space Invaders-style game using C++ and SFML.
    The player moves left and right, shoots enemies, avoids enemy bullets,
    and advances through levels after defeating all enemies.

    C++ concepts demonstrated:
    - Classes: Game class controls the full program.
    - Structs: Player, Enemy, Bullet, ShieldBlock, and Box store game data.
    - Methods/functions: The Game class breaks the program into many methods.
    - Data structures: std::vector stores enemies, bullets, and shields.
    - User interaction: Keyboard input controls movement, shooting, restart, and quit.
*/

namespace
{
    // [Section owner: Manuel]
    // Constants keep game settings in one place so the game is easier to tune.
    constexpr unsigned int WINDOW_WIDTH = 900;
    constexpr unsigned int WINDOW_HEIGHT = 700;

    constexpr float PLAYER_WIDTH = 58.f;
    constexpr float PLAYER_HEIGHT = 22.f;
    constexpr float PLAYER_SPEED = 360.f;

    constexpr float ENEMY_WIDTH = 42.f;
    constexpr float ENEMY_HEIGHT = 26.f;
    constexpr int ENEMY_ROWS = 5;
    constexpr int ENEMY_COLUMNS = 11;

    constexpr float PLAYER_BULLET_SPEED = 560.f;
    constexpr float ENEMY_BULLET_SPEED = 250.f;

    constexpr int STARTING_LIVES = 3;

    // This function keeps a value inside a low/high range.
    // It is used to prevent the player from leaving the screen.
    float clampFloat(float value, float low, float high)
    {
        return std::max(low, std::min(value, high));
    }
}

// [Section owner: Manuel]
// Box represents a rectangle. It is used for simple collision detection.
struct Box
{
    sf::Vector2f position;
    sf::Vector2f size;
};

// [Section owner: Manuel]
// Axis-Aligned Bounding Box collision check.
// Since every object is treated as a rectangle, this is a simple and fast way
// to check whether two objects are touching.
bool intersects(const Box& a, const Box& b)
{
    return a.position.x < b.position.x + b.size.x &&
           a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y &&
           a.position.y + a.size.y > b.position.y;
}

// [Section owner: Justin]
// Bullet stores the position, size, speed, and ownership of each shot.
struct Bullet
{
    sf::Vector2f position;
    sf::Vector2f size{6.f, 16.f};
    float speed = 0.f;
    bool fromPlayer = true;
    bool active = true;

    Box box() const
    {
        return {position, size};
    }
};

// [Section owner: Nabila]
// Enemy stores one alien invader. The alive flag controls whether it is drawn.
struct Enemy
{
    sf::Vector2f position;
    sf::Vector2f size{ENEMY_WIDTH, ENEMY_HEIGHT};
    bool alive = true;
    int points = 10;

    Box box() const
    {
        return {position, size};
    }
};

// [Section owner: Nabila]
// Shields are made of small blocks. Each block can be destroyed separately.
struct ShieldBlock
{
    sf::Vector2f position;
    sf::Vector2f size{14.f, 10.f};
    bool alive = true;

    Box box() const
    {
        return {position, size};
    }
};

// [Section owner: Justin]
// Player stores the user's ship position and size.
struct Player
{
    sf::Vector2f position{WINDOW_WIDTH / 2.f - PLAYER_WIDTH / 2.f, WINDOW_HEIGHT - 70.f};
    sf::Vector2f size{PLAYER_WIDTH, PLAYER_HEIGHT};

    Box box() const
    {
        return {position, size};
    }
};

// [Section owner: Manuel]
// GameState prevents the update loop from continuing after a loss.
enum class GameState
{
    Playing,
    Lost
};

// [Team section]
// The Game class controls the full application.
// This demonstrates object-oriented programming by grouping related data and
// methods inside one class.
class Game
{
public:
    // [Section owner: Nabila]
    // The constructor creates the game window, sets the frame rate, and starts
    // the first game.
    Game()
        : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Space Invaders - SFML"),
          randomEngine(std::random_device{}())
    {
        window.setFramerateLimit(60);
        resetGame();
    }

    // [Section owner: Manuel]
    // Main game loop. It keeps running while the window is open.
    void run()
    {
        sf::Clock clock;

        while (window.isOpen())
        {
            const float deltaTime = clock.restart().asSeconds();

            processEvents();
            update(deltaTime);
            draw();
        }
    }

private:
    sf::RenderWindow window;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<ShieldBlock> shields;

    std::mt19937 randomEngine;

    GameState state = GameState::Playing;
    int lives = STARTING_LIVES;
    int score = 0;
    int level = 1;

    float playerShootCooldown = 0.f;
    float enemyShootTimer = 0.f;
    float enemyDirection = 1.f;
    float enemySpeed = 45.f;

    // [Section owner: Nabila]
    // Resets all important game values back to their starting state.
    void resetGame()
    {
        lives = STARTING_LIVES;
        score = 0;
        level = 1;
        state = GameState::Playing;
        player.position = {WINDOW_WIDTH / 2.f - PLAYER_WIDTH / 2.f, WINDOW_HEIGHT - 70.f};
        enemySpeed = 45.f;
        enemyDirection = 1.f;
        bullets.clear();
        createEnemies();
        createShields();
        updateWindowTitle();
    }

    // [Section owner: Nabila]
    // Starts a new level after all enemies have been defeated.
    // Enemy speed increases to make later levels harder.
    void startNextLevel()
    {
        ++level;
        enemySpeed += 16.f;
        enemyDirection = 1.f;
        bullets.clear();
        createEnemies();
        createShields();
        state = GameState::Playing;
        updateWindowTitle();
    }

    // [Section owner: Nabila]
    // Creates a grid of enemies using nested loops.
    // This shows use of repetition and a vector data structure.
    void createEnemies()
    {
        enemies.clear();

        const float startX = 130.f;
        const float startY = 80.f;
        const float gapX = 58.f;
        const float gapY = 44.f;

        for (int row = 0; row < ENEMY_ROWS; ++row)
        {
            for (int col = 0; col < ENEMY_COLUMNS; ++col)
            {
                Enemy enemy;
                enemy.position = {startX + col * gapX, startY + row * gapY};
                enemy.points = (ENEMY_ROWS - row) * 10;
                enemies.push_back(enemy);
            }
        }
    }

    // [Section owner: Nabila]
    // Creates four destructible shields using many small blocks.
    void createShields()
    {
        shields.clear();

        const std::vector<float> shieldCenters = {180.f, 360.f, 540.f, 720.f};
        const float shieldY = WINDOW_HEIGHT - 190.f;

        for (float centerX : shieldCenters)
        {
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 7; ++col)
                {
                    // The bottom hole gives each shield a simple arch shape.
                    const bool bottomHole = row == 3 && (col == 2 || col == 3 || col == 4);
                    if (bottomHole)
                    {
                        continue;
                    }

                    ShieldBlock block;
                    block.position = {centerX - 49.f + col * block.size.x, shieldY + row * block.size.y};
                    shields.push_back(block);
                }
            }
        }
    }

    // [Section owner: Justin]
    // Reads close-window events from SFML.
    void processEvents()
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
    }

    // [Section owner: Manuel]
    // Updates all game systems once per frame.
    // deltaTime makes movement independent of computer speed.
    void update(float deltaTime)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            window.close();
        }

        if (state != GameState::Playing)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
            {
                resetGame();
            }
            return;
        }

        playerShootCooldown = std::max(0.f, playerShootCooldown - deltaTime);
        enemyShootTimer -= deltaTime;

        updatePlayer(deltaTime);
        updateEnemies(deltaTime);
        updateBullets(deltaTime);
        handleCollisions();
        enemyShoot();
        checkWinOrLose();
        updateWindowTitle();
    }

    // [Section owner: Justin]
    // Handles keyboard movement and player shooting.
    void updatePlayer(float deltaTime)
    {
        float movement = 0.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            movement -= PLAYER_SPEED * deltaTime;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            movement += PLAYER_SPEED * deltaTime;
        }

        player.position.x = clampFloat(player.position.x + movement, 20.f, WINDOW_WIDTH - PLAYER_WIDTH - 20.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && playerShootCooldown <= 0.f)
        {
            Bullet bullet;
            bullet.fromPlayer = true;
            bullet.speed = -PLAYER_BULLET_SPEED;
            bullet.position = {player.position.x + player.size.x / 2.f - bullet.size.x / 2.f, player.position.y - bullet.size.y};
            bullets.push_back(bullet);
            playerShootCooldown = 0.28f;
        }
    }

    // [Section owner: Justin]
    // Moves enemies left and right. When the group hits a side wall, it drops down.
    void updateEnemies(float deltaTime)
    {
        bool shouldDrop = false;

        for (const Enemy& enemy : enemies)
        {
            if (!enemy.alive)
            {
                continue;
            }

            const float nextX = enemy.position.x + enemyDirection * enemySpeed * deltaTime;
            if (nextX < 20.f || nextX + enemy.size.x > WINDOW_WIDTH - 20.f)
            {
                shouldDrop = true;
                break;
            }
        }

        if (shouldDrop)
        {
            enemyDirection *= -1.f;
            for (Enemy& enemy : enemies)
            {
                enemy.position.y += 26.f;
            }
        }
        else
        {
            for (Enemy& enemy : enemies)
            {
                enemy.position.x += enemyDirection * enemySpeed * deltaTime;
            }
        }
    }

    // [Section owner: Manuel]
    // Moves bullets and removes bullets that are inactive or off-screen.
    void updateBullets(float deltaTime)
    {
        for (Bullet& bullet : bullets)
        {
            bullet.position.y += bullet.speed * deltaTime;
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet)
                           { return !bullet.active || bullet.position.y < -30.f || bullet.position.y > WINDOW_HEIGHT + 30.f; }),
            bullets.end());
    }

    // [Section owner: Justin]
    // Chooses a living enemy at random and fires a bullet downward.
    void enemyShoot()
    {
        if (enemyShootTimer > 0.f)
        {
            return;
        }

        std::vector<int> livingEnemyIndexes;
        for (int i = 0; i < static_cast<int>(enemies.size()); ++i)
        {
            if (enemies[i].alive)
            {
                livingEnemyIndexes.push_back(i);
            }
        }

        if (livingEnemyIndexes.empty())
        {
            return;
        }

        std::uniform_int_distribution<int> enemyPicker(0, static_cast<int>(livingEnemyIndexes.size()) - 1);
        const Enemy& shooter = enemies[livingEnemyIndexes[enemyPicker(randomEngine)]];

        Bullet bullet;
        bullet.fromPlayer = false;
        bullet.speed = ENEMY_BULLET_SPEED + level * 18.f;
        bullet.position = {shooter.position.x + shooter.size.x / 2.f - bullet.size.x / 2.f,
                           shooter.position.y + shooter.size.y + 4.f};
        bullets.push_back(bullet);

        std::uniform_real_distribution<float> timerPicker(0.35f, 0.95f);
        enemyShootTimer = timerPicker(randomEngine);
    }

    // [Section owner: Manuel]
    // Handles bullet collisions with enemies, the player, and shield blocks.
    void handleCollisions()
    {
        for (Bullet& bullet : bullets)
        {
            if (!bullet.active)
            {
                continue;
            }

            if (bullet.fromPlayer)
            {
                for (Enemy& enemy : enemies)
                {
                    if (enemy.alive && intersects(bullet.box(), enemy.box()))
                    {
                        enemy.alive = false;
                        bullet.active = false;
                        score += enemy.points;
                        break;
                    }
                }
            }
            else if (intersects(bullet.box(), player.box()))
            {
                bullet.active = false;
                --lives;
                player.position.x = WINDOW_WIDTH / 2.f - PLAYER_WIDTH / 2.f;

                if (lives <= 0)
                {
                    state = GameState::Lost;
                }
            }

            for (ShieldBlock& block : shields)
            {
                if (block.alive && bullet.active && intersects(bullet.box(), block.box()))
                {
                    block.alive = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    // [Section owner: Manuel]
    // Checks whether the user cleared the level or lost the game.
    void checkWinOrLose()
    {
        const bool allEnemiesDefeated = std::all_of(enemies.begin(), enemies.end(), [](const Enemy& enemy)
                                                    { return !enemy.alive; });

        if (allEnemiesDefeated)
        {
            startNextLevel();
            return;
        }

        for (const Enemy& enemy : enemies)
        {
            if (enemy.alive && enemy.position.y + enemy.size.y >= player.position.y)
            {
                state = GameState::Lost;
                return;
            }
        }
    }

    // [Section owner: Denim]
    // The SFML version avoids external font files, so the score is shown in
    // the window title instead of drawing text inside the window.
    void updateWindowTitle()
    {
        std::ostringstream title;
        title << "Space Invaders - Score: " << score << " | Lives: " << lives << " | Level: " << level;

        if (state == GameState::Lost)
        {
            title << " | GAME OVER - Press R to restart";
        }

        window.setTitle(title.str());
    }

    // [Section owner: Denim]
    // Draws the whole frame in the correct order.
    void draw()
    {
        window.clear(sf::Color(8, 10, 24));

        drawStars();
        drawPlayer();
        drawEnemies();
        drawBullets();
        drawShields();
        drawHudBars();

        window.display();
    }

    // [Section owner: Denim]
    // Draws a simple star field background.
    void drawStars()
    {
        sf::CircleShape star(1.2f);
        star.setFillColor(sf::Color(160, 160, 190));

        for (int i = 0; i < 70; ++i)
        {
            const float x = static_cast<float>((i * 137) % WINDOW_WIDTH);
            const float y = static_cast<float>((i * 83) % (WINDOW_HEIGHT - 100));
            star.setPosition({x, y});
            window.draw(star);
        }
    }

    // [Section owner: Denim]
    // Draws the player's ship using simple rectangles.
    void drawPlayer()
    {
        sf::RectangleShape body(player.size);
        body.setPosition(player.position);
        body.setFillColor(sf::Color(60, 210, 115));
        window.draw(body);

        sf::RectangleShape cannon({12.f, 18.f});
        cannon.setPosition({player.position.x + player.size.x / 2.f - 6.f, player.position.y - 14.f});
        cannon.setFillColor(sf::Color(95, 240, 150));
        window.draw(cannon);
    }

    // [Section owner: Denim]
    // Draws all living enemies.
    void drawEnemies()
    {
        for (const Enemy& enemy : enemies)
        {
            if (!enemy.alive)
            {
                continue;
            }

            sf::RectangleShape body(enemy.size);
            body.setPosition(enemy.position);
            body.setFillColor(sf::Color(210, 80, 240));
            window.draw(body);

            sf::RectangleShape eye({6.f, 6.f});
            eye.setFillColor(sf::Color(8, 10, 24));
            eye.setPosition({enemy.position.x + 9.f, enemy.position.y + 8.f});
            window.draw(eye);
            eye.setPosition({enemy.position.x + enemy.size.x - 15.f, enemy.position.y + 8.f});
            window.draw(eye);
        }
    }

    // [Section owner: Denim]
    // Draws player and enemy bullets in different colors.
    void drawBullets()
    {
        for (const Bullet& bullet : bullets)
        {
            sf::RectangleShape shape(bullet.size);
            shape.setPosition(bullet.position);
            shape.setFillColor(bullet.fromPlayer ? sf::Color(255, 235, 90) : sf::Color(255, 90, 90));
            window.draw(shape);
        }
    }

    // [Section owner: Denim]
    // Draws each shield block that has not been destroyed.
    void drawShields()
    {
        for (const ShieldBlock& block : shields)
        {
            if (!block.alive)
            {
                continue;
            }

            sf::RectangleShape shape(block.size);
            shape.setPosition(block.position);
            shape.setFillColor(sf::Color(70, 170, 255));
            window.draw(shape);
        }
    }

    // [Section owner: Denim]
    // Draws visual lives and the bottom boundary line.
    void drawHudBars()
    {
        for (int i = 0; i < lives; ++i)
        {
            sf::RectangleShape lifeIcon({22.f, 8.f});
            lifeIcon.setPosition({20.f + i * 30.f, 24.f});
            lifeIcon.setFillColor(sf::Color(60, 210, 115));
            window.draw(lifeIcon);
        }

        sf::RectangleShape ground({static_cast<float>(WINDOW_WIDTH), 3.f});
        ground.setPosition({0.f, WINDOW_HEIGHT - 32.f});
        ground.setFillColor(sf::Color(70, 170, 255));
        window.draw(ground);
    }
};

// [Team section]
// Program entry point.
int main()
{
    Game game;
    game.run();
    return 0;
}
