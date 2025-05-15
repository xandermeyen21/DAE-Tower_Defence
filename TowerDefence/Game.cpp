#include "pch.h"
#include "Game.h"
#include "Tower.h"
#include "EnemyBase.h"
#include "Bullet.h"
#include "utils.h"
#include <fstream> 
#include <sstream>
#include <iostream>
#include "Texture.h"
#include "RangedEnemy.h"
#include "BossEnemy.h"
#include "MeleeEnemy.h"
#include "Upgrade.h"
#include <iomanip>

Game::Game(const Window& window)
    : BaseGame{ window }
    , m_pTower{ nullptr }
    , m_GameState{ GameState::Playing }
    , m_CurrentWave{ 1 }
    , m_EnemiesKilled{ 0 }
    , m_EnemiesRequiredForWave{ 5 }
    , m_WaveInProgress{ true }
    , m_EnemySpawnTimer{ 0.f }
    , m_EnemySpawnInterval{ 2.f }
    , m_MaxEnemies{ 10 }
    , m_TowerHealth{ 100 }
    , m_MaxTowerHealth{ 100 }
    , m_RangedEnemyChance{ 20 }
    , m_BossSpawned{ false }
    , m_SelectedUpgrade{ 0 }
    , m_Width{ window.width }
    , m_Height{ window.height }
    , m_EnemiesSpawnedInWave{ 0 }
    , m_IsBossWave{ false }
    , m_BossWavesCompleted{ 0 }
    , m_EnemyDamageMultiplier{ 1.0f }
    , m_EnemyAttackSpeedMultiplier{ 1.0f }
    , m_NotificationTimer{ -1.0f }
    , m_HighScore{ 0 }
{
    Initialize();
    LoadHighScore(); 
}

Game::~Game()
{
    Cleanup();
}

void Game::Initialize()
{
    float towerWidth = 40.f;
    float towerHeight = 60.f;
    float centerX = m_Width / 2.f - towerWidth / 2.f;
    float centerY = m_Height / 2.f - towerHeight / 2.f;

    if (centerX < 0) centerX = 0;
    if (centerY < 0) centerY = 0;
    if (centerX + towerWidth > m_Width) centerX = m_Width - towerWidth;
    if (centerY + towerHeight > m_Height) centerY = m_Height - towerHeight;

    m_pTower = new Tower{ Rectf{centerX, centerY, towerWidth, towerHeight}, 150.f };
    if (!m_pTower) {
        std::cerr << "Failed to create tower" << std::endl;
        return;
    }

    m_MainFontPath = "Resources/Baloo2.ttf";
    m_HeaderFontPath = "Resources/Bungee.ttf";

    InitializeFonts();

    try {
        m_pDamageCardTexture = new Texture("Resources/DamageUpgrade.png");
        m_pAttackSpeedCardTexture = new Texture("Resources/AttackSpeedUpgrade.png");
        m_pRangeCardTexture = new Texture("Resources/RangeUpgrade.png");
        m_pRepairCardTexture = new Texture("Resources/HealthUpgrade.png");
        m_pRicocheetTexture = new Texture("Resources/RicochetUpgrade.png");
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load textures: " << e.what() << std::endl;
    }

    SetupUpgradeOptions();
}


void Game::SetupUpgradeOptions()
{
 
    for (Upgrade* upgrade : m_AvailableUpgrades) {
        delete upgrade;
    }
    m_AvailableUpgrades.clear();

    Upgrade* dmg = Upgrade::CreateDamageUpgrade(2.0f);
    Upgrade* spd = Upgrade::CreateAttackSpeedUpgrade(0.3f);
    Upgrade* rng = Upgrade::CreateRangeUpgrade(30.0f);
    Upgrade* rep = Upgrade::CreateRepairUpgrade(25.0f);
    Upgrade* rico = Upgrade::CreateRicochetUpgrade(1.f);

    if (m_pDamageCardTexture) dmg->SetTexture(m_pDamageCardTexture);
    if (m_pAttackSpeedCardTexture) spd->SetTexture(m_pAttackSpeedCardTexture);
    if (m_pRangeCardTexture) rng->SetTexture(m_pRangeCardTexture);
    if (m_pRepairCardTexture) rep->SetTexture(m_pRepairCardTexture);
    if (m_pRicocheetTexture) rico->SetTexture(m_pRicocheetTexture);

    for (Upgrade* upg : { dmg, spd, rng, rep, rico }) {
        upg->SetFontStyle(m_HeaderFontPath, m_MainFontPath,
            m_SmallFontSize + 2, m_SmallFontSize,
            m_HighlightColor, m_NormalColor);
    }

    m_AvailableUpgrades.push_back(dmg);
    m_AvailableUpgrades.push_back(spd);
    m_AvailableUpgrades.push_back(rng);
    m_AvailableUpgrades.push_back(rep);
    m_AvailableUpgrades.push_back(rico);
}



void Game::Cleanup()
{
    delete m_pTower;
    for (EnemyBase* enemy : m_pEnemies)
    {
        delete enemy;
    }
    m_pEnemies.clear();

    for (Upgrade* upgrade : m_AvailableUpgrades)
    {
        delete upgrade;
    }
    m_AvailableUpgrades.clear();

    delete m_pDamageCardTexture;
    delete m_pAttackSpeedCardTexture;
    delete m_pRangeCardTexture;
    delete m_pRepairCardTexture;
    delete m_pRicocheetTexture;

    m_pTower = nullptr;
    m_pDamageCardTexture = nullptr;
    m_pAttackSpeedCardTexture = nullptr;
    m_pRangeCardTexture = nullptr;
    m_pRepairCardTexture = nullptr;
    m_pRicocheetTexture = nullptr;
}

void Game::Update(float elapsedSec)
{
    switch (m_GameState)
    {
    case GameState::Playing:
    {
        m_pTower->Update(elapsedSec, m_pEnemies);
        m_IsBossWave = (m_CurrentWave % 5 == 0);
        m_EnemySpawnTimer += elapsedSec;

       
        bool shouldSpawnEnemy = m_WaveInProgress &&
            (m_EnemiesSpawnedInWave < m_EnemiesRequiredForWave) &&
            (m_pEnemies.size() < m_MaxEnemies) &&
            (m_EnemySpawnTimer >= m_EnemySpawnInterval);

        if (shouldSpawnEnemy)
        {
            m_EnemiesSpawnedInWave++;
            m_EnemySpawnTimer = 0.f;

            if (m_IsBossWave)
            {
                if (!m_BossSpawned)
                {
                    SpawnEnemy(EnemySpawnType::Boss);
                    m_BossSpawned = true;
                }
            }
            else 
            {
                EnemySpawnType type = (rand() % 100 < m_RangedEnemyChance)
                    ? EnemySpawnType::Ranged
                    : EnemySpawnType::Normal;
                SpawnEnemy(type);
            }
        }

        float towerCenterX = m_pTower->GetPosition().left + m_pTower->GetPosition().width / 2.f;
        float towerCenterY = m_pTower->GetPosition().bottom + m_pTower->GetPosition().height / 2.f;

        
        for (size_t i = 0; i < m_pEnemies.size(); )
        {
            m_pEnemies[i]->Update(towerCenterX, towerCenterY, elapsedSec);

          
            if (m_pEnemies[i]->GetType() == EnemyType::Ranged)
            {
                RangedEnemy* ranged = static_cast<RangedEnemy*>(m_pEnemies[i]);
                ranged->ShootIfAble(towerCenterX, towerCenterY, elapsedSec);
            }

            bool enemyWasHit = ProcessBulletCollisions(m_pEnemies[i]);
            if (enemyWasHit)
            {
                delete m_pEnemies[i];
                std::swap(m_pEnemies[i], m_pEnemies.back());
                m_pEnemies.pop_back();
                m_EnemiesKilled++;
            }
            else
            {
                ++i;
            }
        }

       
        for (EnemyBase* enemy : m_pEnemies)
        {
            if (enemy->GetType() == EnemyType::Ranged)
            {
                RangedEnemy* ranged = static_cast<RangedEnemy*>(enemy);
                ranged->UpdateBullets(elapsedSec);

               
                for (Bullet& bullet : ranged->GetBullets())
                {
                    Rectf towerRect = m_pTower->GetPosition();
                    Ellipsef towerEllipse(
                        towerRect.left + towerRect.width / 2.0f,
                        towerRect.bottom + towerRect.height / 2.0f,
                        towerRect.width / 2.0f,
                        towerRect.height / 2.0f
                    );
                    if (bullet.IsActive() && bullet.CheckHit(towerEllipse))
                    {
                        UpdateTowerHealth(-bullet.GetDamage());
                        bullet.Deactivate();
                    }
                }
            }
            else if (enemy->GetType() == EnemyType::Boss)
            {
                BossEnemy* boss = static_cast<BossEnemy*>(enemy);

                
                for (Bullet& bullet : boss->GetBullets())
                {
                    Rectf towerRect = m_pTower->GetPosition();
                    Ellipsef towerEllipse(
                        towerRect.left + towerRect.width / 2.0f,
                        towerRect.bottom + towerRect.height / 2.0f,
                        towerRect.width / 2.0f,
                        towerRect.height / 2.0f
                    );
                    if (bullet.IsActive() && bullet.CheckHit(towerEllipse))
                    {
                        UpdateTowerHealth(-bullet.GetDamage());
                        bullet.Deactivate();
                    }
                }
            }
        }

        CheckWaveComplete();

       
        if (ProcessEnemyAttacks(elapsedSec) || m_TowerHealth <= 0)
        {
            if (m_TowerHealth <= 0 && m_GameState != GameState::GameOver)
            {
                m_GameState = GameState::GameOver;
                for (auto enemy : m_pEnemies) delete enemy;
                m_pEnemies.clear();
            }
        }

       
        if (!m_Notifications.empty())
        {
            for (size_t i = 0; i < m_Notifications.size(); )
            {
                if (m_Notifications[i].second >= 0.0f)
                {
                    m_Notifications[i].second -= elapsedSec;
                    if (m_Notifications[i].second <= 0)
                    {
                        m_Notifications.erase(m_Notifications.begin() + i);
                    }
                    else
                    {
                        i++;
                    }
                }
                else
                {
                    m_Notifications.erase(m_Notifications.begin() + i);
                }
            }
        }
        break;
    }
    case GameState::UpgradeMenu:
    case GameState::GameOver:
        break;
    }

    CleanupBullets();
}


void Game::Draw() const
{
    ClearBackground();
    m_pTower->Draw();
    for (const EnemyBase* enemy : m_pEnemies)
    {
        enemy->Draw();
    }

    switch (m_GameState)
    {
    case GameState::Playing:
    {
        {
            std::stringstream ss;
            ss << "WAVE " << m_CurrentWave;
            Texture waveText(ss.str(), m_HeaderFontPath, m_HeadingFontSize, m_TitleColor);
            waveText.Draw(Vector2f(m_Width / 2.f - waveText.GetWidth() / 2.f, m_Height - 40.f));

            ss.str("");
            ss << "ENEMIES " << m_EnemiesKilled << " / " << m_EnemiesRequiredForWave;
            Texture enemyText(ss.str(), m_MainFontPath, m_NormalFontSize, m_NormalColor);
            enemyText.Draw(Vector2f(m_Width / 2.f - enemyText.GetWidth() / 2.f, m_Height - 70.f));

            if (m_BossWavesCompleted > 0) {
                ss.str("");
                ss << "ENEMY POWER LEVEL: " << m_BossWavesCompleted;

                Color4f powerColor;
                if (m_BossWavesCompleted == 1) powerColor = Color4f{ 1.0f, 0.9f, 0.2f, 1.0f };
                else if (m_BossWavesCompleted == 2) powerColor = Color4f{ 1.0f, 0.7f, 0.2f, 1.0f };
                else if (m_BossWavesCompleted == 3) powerColor = Color4f{ 1.0f, 0.5f, 0.2f, 1.0f };
                else powerColor = Color4f{ 1.0f, 0.3f, 0.2f, 1.0f };

                Texture powerText(ss.str(), m_HeaderFontPath, m_SmallFontSize, powerColor);
                powerText.Draw(Vector2f(m_Width / 2.f - powerText.GetWidth() / 2.f, m_Height - 130.f));
            }
        }

        {
            float leftPadding = 20.f;
            float topPadding = 20.f;
            float lineSpacing = 6.f;
            float y = m_Height - topPadding;

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << m_pTower->GetDamage();
            std::string dmgStr = oss.str();
            oss.str(""); oss.clear();
            oss << std::fixed << std::setprecision(2) << m_pTower->GetAttackSpeed();
            std::string spdStr = oss.str();
            oss.str(""); oss.clear();
            oss << std::fixed << std::setprecision(1) << m_pTower->GetRange();
            std::string rngStr = oss.str();
            oss.str(""); oss.clear();
            oss << std::fixed << std::setprecision(0) << m_pTower->GetRicochetCount();
            std::string ricStr = oss.str();
            oss.str(""); oss.clear();
            oss << std::fixed << std::setprecision(0) << m_TowerHealth;
            std::string healthStr = oss.str();
            oss.str(""); oss.clear();
            oss << std::fixed << std::setprecision(0) << m_MaxTowerHealth;
            std::string maxHealthStr = oss.str();

            std::vector<std::string> statLines = {
                "DAMAGE: " + dmgStr,
                "SPEED: " + spdStr,
                "RANGE: " + rngStr,
                "BOUNCE: " + ricStr,
                "HEALTH: " + healthStr + " / " + maxHealthStr
            };

            for (const auto& line : statLines) {
                Texture statText(line, m_MainFontPath, m_SmallFontSize, m_StatsColor);
                statText.Draw(Vector2f(leftPadding, y - statText.GetHeight()));
                y -= (statText.GetHeight() + lineSpacing);
            }

        }

        {
            Rectf towerRect = m_pTower->GetPosition();
            float barWidth = 100.0f;
            float barHeight = 15.0f;
            float barX = towerRect.left + (towerRect.width - barWidth) / 2.0f;
            float barY = towerRect.bottom + towerRect.height + 15.0f;

            float healthPercent = float(m_TowerHealth) / float(m_MaxTowerHealth);
            healthPercent = std::max(0.0f, std::min(1.0f, healthPercent));

            utils::SetColor(Color4f(0.2f, 0.2f, 0.2f, 1.0f));
            utils::FillRect(Rectf(barX, barY, barWidth, barHeight));

            Color4f healthColor(1.0f - healthPercent, healthPercent, 0.0f, 1.0f);
            utils::SetColor(healthColor);
            utils::FillRect(Rectf(barX, barY, barWidth * healthPercent, barHeight));
        }

        if (!m_Notifications.empty()) {
            float notifY = m_Height - 160.f;
            for (const auto& notification : m_Notifications) {
                float alpha = 1.0f;
                if (notification.second < 1.0f) {
                    alpha = notification.second;
                }
                else if (notification.first.find("WARNING") != std::string::npos) {
                    alpha = 0.7f + 0.3f * sin(SDL_GetTicks() * 0.008f);
                }

                Color4f notifColor;
                if (notification.first.find("WARNING") != std::string::npos) {
                    notifColor = Color4f(1.0f, 0.2f, 0.2f, alpha);
                    Texture notifText(notification.first, m_HeaderFontPath, m_NormalFontSize, notifColor);
                    notifText.Draw(Vector2f(m_Width / 2.f - notifText.GetWidth() / 2.f, notifY));
                }
                else if (notification.first.find("LEVEL") != std::string::npos) {
                    notifColor = Color4f(1.0f, 0.7f, 0.2f, alpha);
                    Texture notifText(notification.first, m_HeaderFontPath, m_NormalFontSize, notifColor);
                    notifText.Draw(Vector2f(m_Width / 2.f - notifText.GetWidth() / 2.f, notifY));
                }
                else {
                    notifColor = Color4f(1.0f, 1.0f, 1.0f, alpha);
                    Texture notifText(notification.first, m_MainFontPath, m_NormalFontSize, notifColor);
                    notifText.Draw(Vector2f(m_Width / 2.f - notifText.GetWidth() / 2.f, notifY));
                }
                notifY -= 30.f;
            }
        }
        break;
    }

    case GameState::UpgradeMenu:
    {
        DrawUpgradeMenu();
        break;
    }

    case GameState::GameOver:
    {
        GameOver();
        break;
    }
    }
}


void Game::SpawnEnemy(EnemySpawnType type)
{
    float spawnX = 0.0f, spawnY = 0.0f;
    float radius = 15.f;

    int edge = rand() % 4;
    switch (edge)
    {
    case 0:
        spawnX = float(rand() % int(m_Width));
        spawnY = m_Height + radius;
        break;
    case 1:
        spawnX = m_Width + radius;
        spawnY = float(rand() % int(m_Height));
        break;
    case 2:
        spawnX = float(rand() % int(m_Width));
        spawnY = -radius;
        break;
    case 3:
        spawnX = -radius;
        spawnY = float(rand() % int(m_Height));
        break;
    }

    EnemyBase* enemy = nullptr;

    switch (type)
    {
    case EnemySpawnType::Normal:
    {
        int baseHp = 1 + m_CurrentWave;
        int hp = baseHp * (1 + m_BossWavesCompleted * 0.3f);

        float speed = 40.f + (m_CurrentWave * 3.f);
        MeleeEnemy* meleeEnemy = new MeleeEnemy(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);

        if (m_BossWavesCompleted > 0) {
            meleeEnemy->m_AttackCooldown = std::max(0.3f, 1.0f / m_EnemyAttackSpeedMultiplier);
        }

        enemy = meleeEnemy;
        break;
    }
    case EnemySpawnType::Ranged:
    {
        int baseHp = 3 + static_cast<int>(m_CurrentWave * 0.7f);
        int hp = baseHp * (1 + m_BossWavesCompleted * 0.3f);
        float speed = 30.f + (m_CurrentWave * 2.f);

        RangedEnemy* rangedEnemy = new RangedEnemy(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);

        if (m_BossWavesCompleted > 0) {
            rangedEnemy->m_BulletDamage *= m_EnemyDamageMultiplier;
            rangedEnemy->m_ShootCooldownMax = std::max(0.4f, 1.0f / m_EnemyAttackSpeedMultiplier);
        }

        enemy = rangedEnemy;
        break;
    }
    case EnemySpawnType::Boss:
    {
        int baseHp = 10 + (m_CurrentWave * 10);
        int hp = baseHp * (1 + m_BossWavesCompleted * 0.2f);

        float speed = 25.f + (m_CurrentWave * 1.5f);
        float bossRadius = radius * 1.8f;
        BossEnemy* bossEnemy = new BossEnemy(Ellipsef(spawnX, spawnY, bossRadius, bossRadius), hp, speed, m_CurrentWave);

        if (m_BossWavesCompleted > 0) {
            bossEnemy->SetBulletDamage(bossEnemy->GetBulletDamage() * m_EnemyDamageMultiplier);
            bossEnemy->SetAttackSpeed(bossEnemy->GetAttackSpeed() * m_EnemyAttackSpeedMultiplier);
        }

        enemy = bossEnemy;
        break;
    }
    }

    if (enemy != nullptr)
    {
        m_pEnemies.push_back(enemy);
    }
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
    switch (m_GameState)
    {
    case GameState::UpgradeMenu:
        switch (e.keysym.sym)
        {
        case SDLK_LEFT:
            m_SelectedUpgrade = (m_SelectedUpgrade - 1 + static_cast<int>(m_AvailableUpgrades.size())) % static_cast<int>(m_AvailableUpgrades.size());
            break;
        case SDLK_RIGHT:
            m_SelectedUpgrade = (m_SelectedUpgrade + 1) % static_cast<int>(m_AvailableUpgrades.size());
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            StartNextWave();
            break;
        }
        break;

    case GameState::GameOver:
        if (e.keysym.sym == SDLK_SPACE || e.keysym.sym == SDLK_RETURN)
        {
            Cleanup();

            m_CurrentWave = 1;
            m_EnemiesKilled = 0;
            m_EnemiesRequiredForWave = 5;
            m_EnemiesSpawnedInWave = 0;
            m_WaveInProgress = true;
            m_GameState = GameState::Playing;
            m_TowerHealth = m_MaxTowerHealth;
            m_BossWavesCompleted = 0;             
            m_EnemyDamageMultiplier = 1.0f;       
            m_EnemyAttackSpeedMultiplier = 1.0f;  

            float towerWidth = 30.f;
            float towerHeight = 50.f;
            float centerX = m_Width / 2.f - towerWidth / 2.f;
            float centerY = m_Height / 2.f - towerHeight / 2.f;
            m_pTower = new Tower{ Rectf{centerX, centerY, towerWidth, towerHeight}, 150.f };
        }
    }
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e) {
    if (m_GameState == GameState::GameOver &&
        (e.keysym.sym == SDLK_SPACE || e.keysym.sym == SDLK_RETURN)) {
        if (m_CurrentWave > m_HighScore) {
            m_HighScore = m_CurrentWave;
            SaveHighScore();
        }

        Cleanup();
        m_CurrentWave = 1;
        m_EnemiesKilled = 0;
        m_EnemiesRequiredForWave = 5;
        m_EnemiesSpawnedInWave = 0;
        m_WaveInProgress = true;
        m_GameState = GameState::Playing;
        m_TowerHealth = m_MaxTowerHealth;
        m_BossWavesCompleted = 0;
        m_EnemyDamageMultiplier = 1.0f;
        m_EnemyAttackSpeedMultiplier = 1.0f;

        float towerWidth = 30.f;
        float towerHeight = 50.f;
        float centerX = m_Width / 2 - towerWidth / 2;
        float centerY = m_Height / 2 - towerHeight / 2;
        m_pTower = new Tower{ Rectf{centerX, centerY, towerWidth, towerHeight}, 150.f };
    }
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
    // Add mouse motion handling if needed
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
    if (m_GameState == GameState::UpgradeMenu)
    {
        float menuTop = m_Height / 2.f + 100.f;
        float menuBottom = m_Height / 2.f - 100.f;
        float menuWidth = 300.f;
        float menuLeft = m_Width / 2.f - menuWidth / 2.f;
        float itemHeight = 40.f;

        if (e.x >= menuLeft && e.x <= menuLeft + menuWidth &&
            e.y >= menuBottom && e.y <= menuTop)
        {
            int clickedItem = static_cast<int>((menuTop - e.y) / itemHeight);
            if (clickedItem >= 0 && clickedItem < static_cast<int>(m_AvailableUpgrades.size()))
            {
                m_SelectedUpgrade = clickedItem;

                if (e.clicks == 2)
                {
                    StartNextWave();
                }
            }
        }
    }
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
    // Add mouse release handling if needed
}

void Game::ClearBackground() const
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Game::CleanupBullets()
{
    std::vector<Bullet>& bullets = m_pTower->GetBullets();
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        bullets.end());
}

bool Game::ProcessBulletCollisions(EnemyBase* enemy)
{
    if (!enemy->IsAlive())
        return false;

    std::vector<Bullet>& bullets = m_pTower->GetBullets();
    bool enemyWasKilled = false;

    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); )
    {
        const auto& shape = enemy->GetShape();
        if (bulletIt->CheckHit(shape))
        {
            enemy->TakeDamage(bulletIt->GetDamage());
            bool enemyKilled = !enemy->IsAlive();

            if (bulletIt->m_RicochetLeft > 0)
            {
                EnemyBase* nextTarget = nullptr;
                float minDist = std::numeric_limits<float>::max();
                Vector2f hitPos = bulletIt->GetPosition();

                std::vector<EnemyBase*> potentialTargets;
                for (EnemyBase* other : m_pEnemies)
                {
                    if (other != enemy && other->IsAlive())
                    {
                        potentialTargets.push_back(other);
                    }
                }

                for (EnemyBase* other : potentialTargets)
                {
                    float dx = other->GetShape().center.x - hitPos.x;
                    float dy = other->GetShape().center.y - hitPos.y;
                    float dist = std::sqrt(dx * dx + dy * dy);

                    if (dist < minDist && dist <= m_pTower->GetRange())
                    {
                        minDist = dist;
                        nextTarget = other;
                    }
                }

                if (nextTarget)
                {
                    float ricochetDamage = bulletIt->GetDamage() * 0.8f;
                    nextTarget->TakeDamage(ricochetDamage);
                    bool nextKilled = !nextTarget->IsAlive();
                    if (nextKilled)
                        enemyWasKilled = true;

                    bullets.emplace_back(
                        hitPos.x, hitPos.y,
                        nextTarget->GetShape().center.x, nextTarget->GetShape().center.y,
                        bulletIt->GetSpeed(),
                        bulletIt->GetDamage() * 0.8f,
                        bulletIt->m_RicochetLeft - 1
                    );
                }
            }

            bulletIt->Deactivate();
            ++bulletIt;

            if (enemyKilled)
            {
                enemyWasKilled = true;
                break;
            }
        }
        else
        {
            ++bulletIt;
        }
    }

    return enemyWasKilled;
}


void Game::DrawUpgradeMenu() const
{
    
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.7f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));

    Texture titleText("WAVE " + std::to_string(m_CurrentWave) + " COMPLETED!", m_HeaderFontPath, m_TitleFontSize, m_TitleColor);
    titleText.Draw(Vector2f(m_Width / 2.f - titleText.GetWidth() / 2.f, m_Height / 2.f + 200.f));
    Texture chooseText("Choose an upgrade:", m_MainFontPath, m_NormalFontSize, m_NormalColor);
    chooseText.Draw(Vector2f(m_Width / 2.f - chooseText.GetWidth() / 2.f, m_Height / 2.f + 150.f));

    float cardWidth = 180.f;
    float cardHeight = 240.f;
    float descriptionPadding = 10.f;
    float slotPadding = 10.f;

    float totalWidth = m_AvailableUpgrades.size() * (cardWidth + slotPadding) - slotPadding;
    float menuLeft = m_Width / 2.f - totalWidth / 2.f;
    float menuMiddle = m_Height / 2.f;

    for (size_t i = 0; i < m_AvailableUpgrades.size(); ++i)
    {
        float cardX = menuLeft + i * (cardWidth + slotPadding);
        float cardY = menuMiddle - cardHeight / 2.f;

        
        float slotX = cardX - slotPadding;
        float slotY = cardY - slotPadding;
        float slotW = cardWidth + 2 * slotPadding;
        float slotH = cardHeight + 2 * slotPadding;
        if (i == m_SelectedUpgrade)
            utils::SetColor(Color4f(1.0f, 0.9f, 0.3f, 0.5f));
        else
            utils::SetColor(Color4f(0.2f, 0.2f, 0.2f, 0.3f));
        utils::FillRect(Rectf(slotX, slotY, slotW, slotH));

        
        m_AvailableUpgrades[i]->Draw(cardX, cardY, cardWidth, cardHeight, i == m_SelectedUpgrade);

        
        /*std::string desc = m_AvailableUpgrades[i]->GetDescription();
        Texture descText(desc, m_MainFontPath, m_SmallFontSize, m_NormalColor);
        float descX = cardX + cardWidth / 2.f - descText.GetWidth() / 2.f;
        float descY = cardY + cardHeight + descriptionPadding;
        descText.Draw(Vector2f(descX, descY));*/
    }

    
    float instructionY = menuMiddle - cardHeight / 2.f - 70.f;
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 0.7f));
    Texture instr1("Use LEFT/RIGHT arrows and ENTER to select", m_MainFontPath, m_SmallFontSize, m_NormalColor);
    instr1.Draw(Vector2f(m_Width / 2.f - instr1.GetWidth() / 2.f, instructionY));
}




bool Game::ProcessEnemyAttacks(float elapsedSec)
{
    bool towerDamaged = false;
    Rectf towerRect = m_pTower->GetPosition();

    for (EnemyBase* enemy : m_pEnemies)
    {
        if (enemy->GetType() == EnemyType::Melee)
        {
            MeleeEnemy* melee = static_cast<MeleeEnemy*>(enemy);
            if (melee->m_AtTower && melee->m_AttackCooldown <= 0.0f)
            {
                int damage = melee->GetAttackDamage();
                UpdateTowerHealth(-damage);
                melee->m_AttackCooldown = 1.0f; 
                towerDamaged = true;
            }
        }
    }
    return towerDamaged;
}

void Game::StartNextWave()
{
    m_BossSpawned = false;

    if (m_GameState == GameState::UpgradeMenu && !m_AvailableUpgrades.empty())
    {
        if (m_SelectedUpgrade < m_AvailableUpgrades.size())
        {
            Upgrade* selected = m_AvailableUpgrades[m_SelectedUpgrade];

            if (selected->GetType() == UpgradeType::REPAIR)
            {
                int healthIncrease = static_cast<int>(selected->GetAmount());
                m_MaxTowerHealth += healthIncrease;
            }
            else
            {
                selected->Apply(*m_pTower);
            }
        }
    }

    for (Upgrade* upgrade : m_AvailableUpgrades)
    {
        delete upgrade;
    }
    m_AvailableUpgrades.clear();

    bool nextWaveBossWave = ((m_CurrentWave + 1) % 5 == 0);

    if (m_CurrentWave % 5 == 0)
    {
        m_BossWavesCompleted++;
        ApplyPostBossWaveUpgrades();

        std::stringstream ss;
        ss << "ENEMIES LEVELED UP! POWER LEVEL " << m_BossWavesCompleted;
        AddNotification(ss.str(), 5.0f);
    }

    if (nextWaveBossWave) {
        std::stringstream ss;
        ss << "WARNING! BOSS WAVE INCOMING!";
        AddNotification(ss.str(), 4.0f);
    }

    UpdateTowerHealth(30);
    m_CurrentWave++;
    m_EnemiesKilled = 0;
    m_EnemiesSpawnedInWave = 0;
    m_EnemiesRequiredForWave = 5 + (m_CurrentWave * 2);
    m_WaveInProgress = true;
    m_BossSpawned = false;

    m_EnemySpawnInterval = std::max(0.5f, 2.0f - (m_CurrentWave * 0.1f));
    m_RangedEnemyChance = std::min(40, 20 + m_CurrentWave);

    for (EnemyBase* enemy : m_pEnemies)
    {
        delete enemy;
    }
    m_pEnemies.clear();

    m_GameState = GameState::Playing;
}


void Game::CheckWaveComplete()
{
    if (m_WaveInProgress && m_EnemiesKilled >= m_EnemiesRequiredForWave && m_pEnemies.empty())
    {
        m_WaveInProgress = false;
        m_GameState = GameState::UpgradeMenu;
        SetupUpgradeOptions();
    }
}

void Game::UpdateTowerHealth(int amount)
{
    m_TowerHealth += amount;
    if (m_TowerHealth > m_MaxTowerHealth)
        m_TowerHealth = m_MaxTowerHealth;
    if (m_TowerHealth < 0)
        m_TowerHealth = 0;
}

void Game::ApplyPostBossWaveUpgrades()
{
    m_EnemyDamageMultiplier += 0.2f;
    m_EnemyAttackSpeedMultiplier += 0.15f;

    m_EnemyDamageMultiplier = std::min(3.0f, std::max(0.5f, m_EnemyDamageMultiplier));
    m_EnemyAttackSpeedMultiplier = std::min(2.5f, std::max(0.5f, m_EnemyAttackSpeedMultiplier));
    m_EnemyDamageMultiplier += 0.2f;
   
    m_EnemyAttackSpeedMultiplier += 0.15f;

    m_EnemyDamageMultiplier = std::min(m_EnemyDamageMultiplier, 3.0f);
    m_EnemyAttackSpeedMultiplier = std::min(m_EnemyAttackSpeedMultiplier, 2.5f);

    std::cout << "Boss wave " << m_BossWavesCompleted << " completed! Enemies upgraded:" << std::endl;
    std::cout << "- Damage multiplier: " << m_EnemyDamageMultiplier << std::endl;
    std::cout << "- Attack speed multiplier: " << m_EnemyAttackSpeedMultiplier << std::endl;

    int healthBonus = m_MaxTowerHealth * 0.1f; 
    m_MaxTowerHealth += healthBonus;
    m_TowerHealth += healthBonus;

    m_pTower->IncreaseDamage(m_pTower->GetDamage() * 0.05f); 
}

void Game::GameOver() const {
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.8f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));

    std::string highscoreText = "Highscore: Wave " + std::to_string(m_HighScore);
    Texture highscoreTexture(highscoreText, m_MainFontPath, m_NormalFontSize, m_TitleColor);
    highscoreTexture.Draw(Vector2f(m_Width / 2 - highscoreTexture.GetWidth() / 2, m_Height / 2 - 100));

    utils::SetColor(Color4f(1.0f, 0.2f, 0.2f, 1.0f));
    Texture gameOverText("GAME OVER", m_HeaderFontPath, m_TitleFontSize + 12, m_WarningColor);
    gameOverText.Draw(Vector2f(m_Width / 2 - gameOverText.GetWidth() / 2, m_Height / 2 + 50));

    std::string waveText = "You reached wave " + std::to_string(m_CurrentWave);
    Texture waveReachedText(waveText, m_MainFontPath, m_HeadingFontSize, m_TitleColor);
    waveReachedText.Draw(Vector2f(m_Width / 2 - waveReachedText.GetWidth() / 2, m_Height / 2));

    Texture restartText("Press ENTER or SPACE to restart", m_MainFontPath, m_NormalFontSize, m_NormalColor);
    restartText.Draw(Vector2f(m_Width / 2 - restartText.GetWidth() / 2, m_Height / 2 - 50));
}

void Game::AddNotification(const std::string& text, float duration)
{
    m_Notifications.push_back(std::make_pair(text, duration));
}

void Game::InitializeFonts()
{
    m_TitleFontSize = 28;
    m_HeadingFontSize = 24;
    m_NormalFontSize = 18;
    m_SmallFontSize = 16;

    m_TitleColor = Color4f{ 1.0f, 1.0f, 1.0f, 1.0f };
    m_WarningColor = Color4f{ 1.0f, 0.2f, 0.2f, 1.0f };
    m_HighlightColor = Color4f{ 1.0f, 0.9f, 0.3f, 1.0f };
    m_NormalColor = Color4f{ 1.0f, 1.0f, 1.0f, 0.8f };
    m_StatsColor = Color4f{ 0.8f, 0.8f, 1.0f, 1.0f };
}

void Game::LoadHighScore() {
    std::ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> m_HighScore;
        inFile.close();
    }
}

void Game::SaveHighScore() {
    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << m_HighScore;
        outFile.close();
    }
}