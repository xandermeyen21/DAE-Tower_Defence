#pragma once
#include "BaseGame.h"
#include <vector>

class Tower;
class Enemie;

class Game : public BaseGame
{
public:
    explicit Game(const Window& window);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    Game(Game&& other) = delete;
    Game& operator=(Game&& other) = delete;
    ~Game();

    void Update(float elapsedSec) override;
    void Draw() const override;

    // Event handling
    void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
    void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
    void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
    void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
    void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:
    // FUNCTIONS
    void Initialize();
    void Cleanup();
    void ClearBackground() const;
    void SpawnEnemy();
    bool CheckBulletCollisions(Enemie* enemy);
    void CleanupBullets();

    // MEMBERS
    Tower* m_pTower;
    std::vector<Enemie*> m_pEnemies;

    // Enemy spawning parameters
    float m_EnemySpawnTimer;
    float m_EnemySpawnInterval;
    const int m_MaxEnemies;

    // Game window dimensions for calculations
    float m_Width;
    float m_Height;
};