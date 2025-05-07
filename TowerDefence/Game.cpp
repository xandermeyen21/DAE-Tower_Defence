#include "pch.h"
#include "Game.h"
#include "Tower.h"
#include "EnemyBase.h"
#include "Bullet.h"
#include "utils.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include "Texture.h"
#include "RangedEnemy.h"
#include "BossEnemy.h"
#include "MeleeEnemy.h"
#include "pch.h"

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
{
    Initialize();
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

   
    SetupUpgradeOptions();
}

void Game::SetupUpgradeOptions()
{
    m_UpgradeOptions.clear();

    m_UpgradeOptions.push_back(
        { "Damage Up",
          "Increase tower damage by 1",
          &Tower::UpgradeDamage,
          1.0f });

    m_UpgradeOptions.push_back(
        { "Attack Speed Up",
          "Increase attack speed by 0.2",
          &Tower::UpgradeAttackSpeed,
          0.2f });

    m_UpgradeOptions.push_back(
        { "Range Up",
          "Increase tower range by 25",
          &Tower::UpgradeRange,
          25.0f });

    
    m_UpgradeOptions.push_back(
        { "Repair Tower",
          "Restore 50 tower health",
          nullptr, 
          50.0f });
}

void Game::Cleanup()
{
    delete m_pTower;
    for (EnemyBase* enemy : m_pEnemies)
    {
        delete enemy;
    }
    m_pEnemies.clear();
}

void Game::Update(float elapsedSec)
{
    switch (m_GameState)
    {
    case GameState::Playing:
    {
        m_pTower->Update(elapsedSec, m_pEnemies);

        m_EnemySpawnTimer += elapsedSec;
        bool shouldSpawnEnemy = m_WaveInProgress &&
            (m_EnemiesSpawnedInWave < m_EnemiesRequiredForWave) && 
            (m_pEnemies.size() < m_MaxEnemies) &&
            (m_EnemySpawnTimer >= m_EnemySpawnInterval);

        if (shouldSpawnEnemy) {
            m_EnemiesSpawnedInWave++; 
            m_EnemySpawnTimer = 0.f;
            if (m_CurrentWave % 5 == 0 && !m_BossSpawned) {
                SpawnEnemy(EnemySpawnType::Boss);
                m_BossSpawned = true;
            }
            else {
                EnemySpawnType type = (rand() % 100 < m_RangedEnemyChance)
                    ? EnemySpawnType::Ranged
                    : EnemySpawnType::Normal;
                SpawnEnemy(type);
            }
        }

        for (size_t i = 0; i < m_pEnemies.size();)
        {
            float towerCenterX = m_pTower->GetPosition().left + m_pTower->GetPosition().width / 2.f;
            float towerCenterY = m_pTower->GetPosition().bottom + m_pTower->GetPosition().height / 2.f;
            m_pEnemies[i]->Update(towerCenterX, towerCenterY, elapsedSec);

            if (m_pEnemies[i]->HasReachedTarget(towerCenterX, towerCenterY, 10.0f)) {
                UpdateTowerHealth(2); 
                delete m_pEnemies[i];
                std::swap(m_pEnemies[i], m_pEnemies.back());
                m_pEnemies.pop_back();
                continue; 
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

        CheckWaveComplete();
        if (ProcessEnemyAttacks(elapsedSec)) {
            if (m_TowerHealth <= 0) {
                m_GameState = GameState::GameOver;
            }
        }
        break;
    } 

    case GameState::UpgradeMenu:
        break;

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
        std::stringstream ss;
        ss << "Wave: " << m_CurrentWave << "   Enemies: " << m_EnemiesKilled << "/" << m_EnemiesRequiredForWave;
        {
            Texture waveText(ss.str(), "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
            waveText.Draw(Vector2f(10, m_Height - 20));
        }
        {
            std::stringstream ts;
            ts << "Damage: " << m_pTower->GetDamage()
                << "   Attack Speed: " << m_pTower->GetAttackSpeed()
                << "   Range: " << m_pTower->GetRange();
            Texture statsText(ts.str(), "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
            statsText.Draw(Vector2f(10, m_Height - 40));
        }
        {
            Rectf towerRect = m_pTower->GetPosition();
            float barWidth = 60.0f;
            float barHeight = 10.0f;
            float barX = towerRect.left + (towerRect.width - barWidth) / 2.0f;
            float barY = towerRect.bottom + towerRect.height + 8.0f;

            float healthPercent = float(m_TowerHealth) / float(m_MaxTowerHealth);
            if (healthPercent < 0.0f) healthPercent = 0.0f;
            if (healthPercent > 1.0f) healthPercent = 1.0f;

            
            utils::SetColor(Color4f(0.2f, 0.2f, 0.2f, 1.0f));
            utils::FillRect(Rectf(barX, barY, barWidth, barHeight));

            
            Color4f healthColor(1.0f - healthPercent, healthPercent, 0.0f, 1.0f);
            utils::SetColor(healthColor);
            utils::FillRect(Rectf(barX, barY, barWidth * healthPercent, barHeight));
        }
        {
            std::stringstream hs;
            hs << "Tower Health: " << m_TowerHealth << " / " << m_MaxTowerHealth;
            Texture healthText(hs.str(), "ShortBaby.ttf", 20, Color4f{ 1.0f, 0.4f, 0.4f, 1.0f });
            healthText.Draw(Vector2f(10, m_Height - 60));
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
    float spawnX, spawnY;
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
        int hp = 1 + m_CurrentWave;
        float speed = 40.f + (m_CurrentWave * 3.f);
        enemy = new MeleeEnemy(Ellipsef(spawnX , spawnY , radius, radius), hp, speed);
        break;
    }
    case EnemySpawnType::Ranged:
    {
        int hp = 3 + static_cast<int>(m_CurrentWave * 0.7f);
        float speed = 30.f + (m_CurrentWave * 2.f); 
        enemy = new RangedEnemy(Ellipsef(spawnX , spawnY , radius, radius), hp, speed);
        break;
    }
    case EnemySpawnType::Boss:
    {
        int hp = 10 + (m_CurrentWave * 10);
        float speed = 25.f + (m_CurrentWave * 1.5f); 
        float bossRadius = radius * 1.8f; 
        enemy = new BossEnemy(Ellipsef(spawnX , spawnY , bossRadius, bossRadius), hp, speed, m_CurrentWave);
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
        case SDLK_UP:
            m_SelectedUpgrade = (m_SelectedUpgrade - 1 + static_cast<int>(m_UpgradeOptions.size())) % static_cast<int>(m_UpgradeOptions.size());
            break;
        case SDLK_DOWN:
            m_SelectedUpgrade = (m_SelectedUpgrade + 1) % static_cast<int>(m_UpgradeOptions.size());
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

            
            float towerWidth = 30.f;
            float towerHeight = 50.f;
            float centerX = m_Width / 2.f - towerWidth / 2.f;
            float centerY = m_Height / 2.f - towerHeight / 2.f;
            m_pTower = new Tower{ Rectf{centerX, centerY, towerWidth, towerHeight}, 150.f };
        }
        break;
    }
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
    // Add key handling if needed
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
        float itemHeight = 40.f;


        if (e.y >= menuBottom && e.y <= menuTop)
        {
            int clickedItem = static_cast<int>((menuTop - e.y) / itemHeight);
            if (clickedItem >= 0 && clickedItem < static_cast<int>(m_UpgradeOptions.size()))
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
    std::vector<size_t> bulletsToRemove;

   
    for (size_t i = 0; i < bullets.size(); ++i)
    {
        if (bullets[i].CheckHit(enemy->GetShape()))
        {
            
            bool enemyKilled = enemy->TakeDamage(bullets[i].GetDamage());

            
            bulletsToRemove.push_back(i);

            
            if (enemyKilled)
            {
                
                for (auto it = bulletsToRemove.rbegin(); it != bulletsToRemove.rend(); ++it)
                {
                    bullets.erase(bullets.begin() + *it);
                }
                return true;
            }
        }
    }

    //
    for (auto it = bulletsToRemove.rbegin(); it != bulletsToRemove.rend(); ++it)
    {
        bullets.erase(bullets.begin() + *it);
    }

    return false;
}

void Game::DrawUpgradeMenu() const
{
    
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.7f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));

    
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    {
        Texture titleText("WAVE " + std::to_string(m_CurrentWave) + " COMPLETED!", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
        titleText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f + 150.f));
    }
    {
        Texture chooseText("Choose an upgrade:", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
        chooseText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f + 130.f));
    }


    
    float yPos = m_Height / 2.f + 100.f;
    for (size_t i = 0; i < m_UpgradeOptions.size(); ++i)
    {
        
        if (i == m_SelectedUpgrade)
        {
            utils::SetColor(Color4f(1.0f, 1.0f, 0.0f, 1.0f));
            utils::FillRect(Rectf(m_Width / 2.f - 150.f, yPos - 30.f, 300.f, 40.f));
            utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));
        }

        
        Texture nameText(m_UpgradeOptions[i].name, "ShortBaby.ttf", 18, Color4f{ 0.0f, 0.0f, 0.0f, 1.0f });
        nameText.Draw(Vector2f(m_Width / 2.f - 140.f, yPos - 5.f));
        Texture descText(m_UpgradeOptions[i].description, "ShortBaby.ttf", 14, Color4f{ 0.0f, 0.0f, 0.0f, 1.0f });
        descText.Draw(Vector2f(m_Width / 2.f - 140.f, yPos - 25.f));

        yPos -= 40.f;
    }

    
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 0.7f));
    Texture instr1("Use UP/DOWN arrows and ENTER to select", "ShortBaby.ttf", 16, Color4f{ 1.0f, 1.0f, 1.0f, 0.7f });
    instr1.Draw(Vector2f(m_Width / 2.f - 150.f, m_Height / 2.f - 130.f));
    Texture instr2("Or click/double-click to choose upgrade", "ShortBaby.ttf", 16, Color4f{ 1.0f, 1.0f, 1.0f, 0.7f });
    instr2.Draw(Vector2f(m_Width / 2.f - 150.f, m_Height / 2.f - 150.f));
}

void Game::CheckWaveComplete()
{
    bool allEnemiesSpawned = (m_EnemiesSpawnedInWave >= m_EnemiesRequiredForWave);
    bool noEnemiesLeft = m_pEnemies.empty();

    if (m_WaveInProgress && allEnemiesSpawned && noEnemiesLeft) {
        m_WaveInProgress = false;
        m_GameState = GameState::UpgradeMenu;
    }
}

void Game::StartNextWave()
{
    m_EnemiesSpawnedInWave = 0;
    if (m_SelectedUpgrade == 3) { 
        m_TowerHealth += static_cast<int>(m_UpgradeOptions[m_SelectedUpgrade].upgradeAmount);
        if (m_TowerHealth > m_MaxTowerHealth) {
            m_TowerHealth = m_MaxTowerHealth;
        }
    }
    else if (m_UpgradeOptions[m_SelectedUpgrade].upgradeFunction != nullptr) {
        
        UpgradeOption& option = m_UpgradeOptions[m_SelectedUpgrade];
        (m_pTower->*(option.upgradeFunction))(option.upgradeAmount);
    }

    m_CurrentWave++;
    m_BossSpawned = false;

    if (m_CurrentWave % 5 == 0) {
        m_EnemiesRequiredForWave = 1;
        m_RangedEnemyChance = 0;
    }
    else {
        m_EnemiesRequiredForWave = 5 + (m_CurrentWave * 2);
        m_RangedEnemyChance = 20 + (m_CurrentWave / 3 * 5);
    }

    m_EnemiesKilled = 0;

    m_EnemySpawnInterval = std::max(0.5f, 2.0f - (m_CurrentWave * 0.1f));


    m_GameState = GameState::Playing;
    m_WaveInProgress = true;
}

void Game::GameOver() const
{
    
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.8f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));

    utils::SetColor(Color4f(1.0f, 0.2f, 0.2f, 1.0f));
    {
        Texture overText("GAME OVER", "ShortBaby.ttf", 32, Color4f{ 1.0f, 0.2f, 0.2f, 1.0f });
        overText.Draw(Vector2f(m_Width / 2.f - 60.f, m_Height / 2.f + 20.f));
    }
    {
        Texture reachedText("You reached wave: " + std::to_string(m_CurrentWave), "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
        reachedText.Draw(Vector2f(m_Width / 2.f - 120.f, m_Height / 2.f - 10.f));
    }
    {
        Texture restartText("Press SPACE to restart", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
        restartText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f - 40.f));
    }

}

bool Game::ProcessEnemyAttacks(float elapsedSec)
{
    bool damageTaken = false;
    Rectf towerPos = m_pTower->GetPosition();

    for (EnemyBase* enemy : m_pEnemies) {
        if (enemy->Attack(elapsedSec, towerPos)) {
            if (enemy->GetType() == EnemyType::MELEE) {
                UpdateTowerHealth(2);
                damageTaken = true;
            }
        }

        
        std::vector<Bullet>* bullets = nullptr;
        if (enemy->GetType() == EnemyType::RANGED) {
            bullets = &static_cast<RangedEnemy*>(enemy)->GetBullets();
        }
        else if (enemy->GetType() == EnemyType::BOSS) {
            bullets = &static_cast<BossEnemy*>(enemy)->GetBullets();
        }

        if (bullets) {
            for (size_t i = 0; i < bullets->size();) {
                Bullet& bullet = (*bullets)[i];
                
                Ellipsef towerEllipse = RectToEllipse(m_pTower->GetPosition());
                if (bullet.CheckHit(towerEllipse)) {
                    UpdateTowerHealth(bullet.GetDamage());
                    bullets->erase(bullets->begin() + i);
                    damageTaken = true;
                }
                else {
                    ++i;
                }
            }
        }
    }
    return damageTaken;
}

void Game::UpdateTowerHealth(int damage)
{
    m_TowerHealth -= damage;
    if (m_TowerHealth < 0) m_TowerHealth = 0;
}

Ellipsef RectToEllipse(const Rectf& rect)
{
    float centerX = rect.left + rect.width / 2.0f;
    float centerY = rect.bottom + rect.height / 2.0f;
    
    float radius = std::min(rect.width, rect.height) / 2.0f;
    return Ellipsef{ centerX, centerY, radius, radius };
}