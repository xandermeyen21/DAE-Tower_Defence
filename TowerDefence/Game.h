#pragma once
#include <vector>
#include <string>
#include "BaseGame.h"

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

class Tower;
class EnemyBase;
class Texture;
class Upgrade;

class Game : public BaseGame
{
public:
    Game(const Window& window);
    ~Game();

    void Update(float elapsedSec) override;
    void Draw() const override;

    void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
    void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
    void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
    void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
    void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:
    void Initialize();
    void Cleanup();
    void ClearBackground() const;
    void CleanupBullets();
    void GameOver() const;

    void SpawnEnemy(EnemySpawnType type);
    bool ProcessBulletCollisions(EnemyBase* enemy);
    bool ProcessEnemyAttacks(float elapsedSec);
    void CheckWaveComplete();
    void StartNextWave();
    void UpdateTowerHealth(int amount);
    void SetupUpgradeOptions();
    void DrawUpgradeMenu() const;
    void ApplyPostBossWaveUpgrades();

    void AddNotification(const std::string& text, float duration);

    Tower* m_pTower;
    std::vector<EnemyBase*> m_pEnemies;
    GameState m_GameState;

    int m_CurrentWave;
    int m_EnemiesKilled;
    int m_EnemiesRequiredForWave;
    int m_EnemiesSpawnedInWave;
    int m_BossWavesCompleted;
    int m_TowerHealth;
    int m_MaxTowerHealth;

    bool m_WaveInProgress;
    bool m_BossSpawned;
    bool m_IsBossWave;

    float m_EnemySpawnTimer;
    float m_EnemySpawnInterval;
    float m_NotificationTimer;
    float m_Width;
    float m_Height;
    float m_EnemyDamageMultiplier;
    float m_EnemyAttackSpeedMultiplier;

    int m_RangedEnemyChance;
    int m_MaxEnemies;

    std::vector<std::pair<std::string, float>> m_Notifications;

    int m_SelectedUpgrade;
    std::vector<Upgrade> m_AvailableUpgrades;

    Texture* m_pDamageCardTexture;
    Texture* m_pAttackSpeedCardTexture;
    Texture* m_pRangeCardTexture;
    Texture* m_pRepairCardTexture;
    Texture* m_pRicocheetTexture;

    void InitializeFonts();
    int m_TitleFontSize;
    int m_HeadingFontSize;
    int m_NormalFontSize;
    int m_SmallFontSize;

    Color4f m_TitleColor;
    Color4f m_WarningColor;
    Color4f m_HighlightColor;
    Color4f m_NormalColor;
    Color4f m_StatsColor;

    std::string m_MainFontPath;    
    std::string m_HeaderFontPath;  
};