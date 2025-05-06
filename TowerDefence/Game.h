#pragma once
#include "BaseGame.h"
#include <vector>

class Tower;
class EnemyBase;

enum class GameState
{
    Playing,
    UpgradeMenu,
    GameOver
};

enum class EnemySpawnType
{
    Normal,
    Ranged,
    Boss
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
    void SpawnEnemy(EnemySpawnType type = EnemySpawnType::Normal);
    bool ProcessBulletCollisions(EnemyBase* enemy);
    bool ProcessEnemyAttacks(float elapsedSec);
    void CleanupBullets();
    void DrawUpgradeMenu() const;
    void SetupUpgradeOptions();
    void StartNextWave();
    void CheckWaveComplete();
    void GameOver() const;
    void UpdateTowerHealth(int damage);

    // MEMBERS
    Tower* m_pTower;
    std::vector<EnemyBase*> m_pEnemies;

    GameState m_GameState;
    int m_CurrentWave;
    int m_EnemiesKilled;
    int m_EnemiesRequiredForWave;
    bool m_WaveInProgress;
    int m_TowerHealth; 
    int m_MaxTowerHealth;

    // Enemy spawning
    float m_EnemySpawnTimer;
    float m_EnemySpawnInterval;
    const int m_MaxEnemies;
    int m_RangedEnemyChance; 
    bool m_BossSpawned; 

    std::vector<UpgradeOption> m_UpgradeOptions;
    int m_SelectedUpgrade;

    
    float m_Width;
    float m_Height;
};
