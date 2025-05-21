#pragma once
#include "BaseGame.h"
#include <vector>
//#include "Enemy.h"
#include <string>

class Tower;
class EnemyBase;
class Bullet;
class Texture;
class Upgrade;

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

class Game : public BaseGame
{
public:
    explicit Game(const Window& window);
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
    ~Game() override;

    void Update(float elapsedSec) override;
    void Draw() const override;
    void OnWindowResize(float newWidth, float newHeight);

    // Override IsGameRunning to implement exit functionality
    bool IsGameRunning() const override;

    void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
    void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
    void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
    void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
    void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:
    enum class TextType {
        Normal,
        Title,
        Heading
    };

    // FUNCTIONS
    void Initialize();
    void Cleanup();
    void ClearBackground() const;
    void DrawUpgradeMenu() const;
    void GameOver() const;
    void SpawnEnemy(EnemySpawnType type);
    void CleanupBullets();
    bool ProcessBulletCollisions(EnemyBase* enemy);
    bool ProcessEnemyAttacks(float elapsedSec);
    void StartNextWave();
    void ApplyPostBossWaveUpgrades();
    void RestartGame();
    void SetupUpgradeOptions();
    void AddNotification(const std::string& text, float duration);
    void InitializeFonts();
    void UpdateTowerHealth(int amount);
    void CheckWaveComplete();
    void LoadHighScore();
    void SaveHighScore() const;

    // DATA MEMBERS
    Tower* m_pTower;
    std::vector<EnemyBase*> m_pEnemies;
    GameState m_GameState;
    int m_CurrentWave;
    int m_EnemiesKilled;
    int m_EnemiesRequiredForWave;
    bool m_WaveInProgress;
    float m_EnemySpawnTimer;
    float m_EnemySpawnInterval;
    int m_MaxEnemies;
    int m_TowerHealth;
    int m_MaxTowerHealth;
    int m_RangedEnemyChance;
    bool m_BossSpawned;
    std::vector<Upgrade*> m_AvailableUpgrades;
    int m_SelectedUpgrade;
    float m_Width;
    float m_Height;
    int m_EnemiesSpawnedInWave;
    bool m_IsBossWave;
    int m_BossWavesCompleted;
    float m_EnemyDamageMultiplier;
    float m_EnemyAttackSpeedMultiplier;
    float m_NotificationTimer;
    int m_HighScore;
    int m_Score;
    float m_AspectRatio;
    std::vector<std::pair<std::string, float>> m_Notifications;
    Texture* m_pDamageCardTexture;
    Texture* m_pAttackSpeedCardTexture;
    Texture* m_pRangeCardTexture;
    Texture* m_pRepairCardTexture;
    Texture* m_pRicocheetTexture;
    Texture* m_pBackgroundTexture;
    std::string m_MainFontPath;
    std::string m_HeaderFontPath;
    int m_TitleFontSize;
    int m_HeadingFontSize;
    int m_NormalFontSize;
    int m_SmallFontSize;
    Color4f m_TitleColor;
    Color4f m_WarningColor;
    Color4f m_HighlightColor;
    Color4f m_NormalColor;
    Color4f m_StatsColor;

    bool m_bRunning;
};