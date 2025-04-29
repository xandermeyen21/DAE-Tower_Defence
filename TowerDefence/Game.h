#pragma once
#include "BaseGame.h"
#include <vector>

class Tower;
class Enemie;

enum class GameState
{
    Playing,
    UpgradeMenu,
    GameOver
};

struct UpgradeOption
{
    std::string name;
    std::string description;
    void (Tower::* upgradeFunction)(float);
    float upgradeAmount;
};

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
    bool ProcessBulletCollisions(Enemie* enemy);
    void CleanupBullets();
    void DrawUpgradeMenu() const;
    void SetupUpgradeOptions();
    void StartNextWave();
    void CheckWaveComplete();
    void GameOver() const;

    // MEMBERS
    Tower* m_pTower;
    std::vector<Enemie*> m_pEnemies;

    GameState m_GameState;
    int m_CurrentWave;
    int m_EnemiesKilled;
    int m_EnemiesRequiredForWave;
    bool m_WaveInProgress;

  
    float m_EnemySpawnTimer;
    float m_EnemySpawnInterval;
    const int m_MaxEnemies;


    std::vector<UpgradeOption> m_UpgradeOptions;
    int m_SelectedUpgrade;

    
    float m_Width;
    float m_Height;
};