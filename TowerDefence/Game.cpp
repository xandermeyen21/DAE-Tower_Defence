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
#include "Upgrade.h"

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

    m_pDamageCardTexture = new Texture("DamageUpgrade.png");
    m_pAttackSpeedCardTexture = new Texture("AttackSpeedUpgrade.png");
    m_pRangeCardTexture = new Texture("RangeUpgrade.png");
    m_pRepairCardTexture = new Texture("HealthUpgrade.png");
    m_pRicocheetTexture = new Texture("RicochetUpgrade.png");

    SetupUpgradeOptions();
}

void Game::SetupUpgradeOptions()
{
    m_AvailableUpgrades.clear();

    Upgrade dmg = Upgrade::CreateDamageUpgrade(2.0f);
    dmg.SetTexture(m_pDamageCardTexture);

    Upgrade spd = Upgrade::CreateAttackSpeedUpgrade(0.3f);
    spd.SetTexture(m_pAttackSpeedCardTexture);

    Upgrade rng = Upgrade::CreateRangeUpgrade(30.0f);
    rng.SetTexture(m_pRangeCardTexture);

    Upgrade rep = Upgrade::CreateRepairUpgrade(75.0f);
    rep.SetTexture(m_pRepairCardTexture);

    Upgrade rico = Upgrade::CreateRicochetUpgrade(1.f);
    rico.SetTexture(m_pRicocheetTexture);

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

    delete m_pDamageCardTexture;
    delete m_pAttackSpeedCardTexture;
    delete m_pRangeCardTexture;
    delete m_pRepairCardTexture;
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

        if (shouldSpawnEnemy) {
            m_EnemiesSpawnedInWave++;
            m_EnemySpawnTimer = 0.f;

            if (m_IsBossWave) {
                if (!m_BossSpawned) {
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
            else {
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

            if (m_pEnemies[i]->GetType() == EnemyType::RANGED) {
                RangedEnemy* ranged = static_cast<RangedEnemy*>(m_pEnemies[i]);
                ranged->ShootIfAble(towerCenterX, towerCenterY, elapsedSec);
            }

            bool enemyWasHit = ProcessBulletCollisions(m_pEnemies[i]);
            if (enemyWasHit) {
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

        for (EnemyBase* enemy : m_pEnemies) {
            if (enemy->GetType() == EnemyType::RANGED) {
                RangedEnemy* ranged = static_cast<RangedEnemy*>(enemy);
                ranged->UpdateBullets(elapsedSec);

                for (Bullet& bullet : ranged->GetBullets()) {
                    Rectf towerRect = m_pTower->GetPosition();
                    Ellipsef towerEllipse(
                        towerRect.left + towerRect.width / 2.0f,
                        towerRect.bottom + towerRect.height / 2.0f,
                        towerRect.width / 2.0f,
                        towerRect.height / 2.0f
                    );
                    if (bullet.IsActive() && bullet.CheckHit(towerEllipse)) {
                        UpdateTowerHealth(-bullet.GetDamage());
                        bullet.Deactivate();
                    }
                }
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
        int hp = 1 + m_CurrentWave;
        float speed = 40.f + (m_CurrentWave * 3.f);
        enemy = new MeleeEnemy(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);
        break;
    }
    case EnemySpawnType::Ranged:
    {
        int hp = 3 + static_cast<int>(m_CurrentWave * 0.7f);
        float speed = 30.f + (m_CurrentWave * 2.f);
        enemy = new RangedEnemy(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);
        break;
    }
    case EnemySpawnType::Boss:
    {
        int hp = 10 + (m_CurrentWave * 10);
        float speed = 25.f + (m_CurrentWave * 1.5f);
        float bossRadius = radius * 1.8f;
        enemy = new BossEnemy(Ellipsef(spawnX, spawnY, bossRadius, bossRadius), hp, speed, m_CurrentWave);
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
    std::vector<size_t> bulletsToRemove;

    for (size_t i = 0; i < bullets.size(); ++i)
    {
        if (bullets[i].CheckHit(enemy->GetShape()))
        {
            bool enemyKilled = enemy->TakeDamage(bullets[i].GetDamage());

            if (bullets[i].m_RicochetLeft > 0) 
            {
                EnemyBase* nextTarget = nullptr;
                float minDist = std::numeric_limits<float>::max();
                Vector2f hitPos = bullets[i].GetPosition();

                for (EnemyBase* other : m_pEnemies)
                {
                    if (other == enemy || !other->IsAlive()) continue;
                    float dx = other->GetShape().center.x - hitPos.x;
                    float dy = other->GetShape().center.y - hitPos.y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        nextTarget = other;
                    }
                }

                if (nextTarget)
                {
                    bullets.emplace_back(
                        hitPos.x, hitPos.y,
                        nextTarget->GetShape().center.x, nextTarget->GetShape().center.y,
                        bullets[i].GetSpeed(),
                        bullets[i].GetDamage(),
                        bullets[i].m_RicochetLeft - 1
                    );
                }
            }
            
            bulletsToRemove.push_back(i);
            if (enemyKilled)
                return true; 
        }
    }

    
    for (auto it = bulletsToRemove.rbegin(); it != bulletsToRemove.rend(); ++it)
        bullets.erase(bullets.begin() + *it);

    return false;
}


void Game::DrawUpgradeMenu() const
{
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.7f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));

    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    Texture titleText("WAVE " + std::to_string(m_CurrentWave) + " COMPLETED!", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
    titleText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f + 150.f));

    Texture chooseText("Choose an upgrade:", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
    chooseText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f + 130.f));

    float cardWidth = 180.f;
    float cardHeight = 240.f;
    float slotPadding = 10.f;

    float totalWidth = m_AvailableUpgrades.size() * (cardWidth + slotPadding) - slotPadding;
    float menuTop = m_Height / 2.f + 100.f;
    float menuLeft = m_Width / 2.f - totalWidth / 2.f;

    for (size_t i = 0; i < m_AvailableUpgrades.size(); ++i)
    {
        float slotX = menuLeft + i * (cardWidth + slotPadding) - slotPadding;
        float slotY = menuTop - cardHeight - slotPadding;
        float slotW = cardWidth + 2 * slotPadding;
        float slotH = cardHeight + 2 * slotPadding;

        if (i == m_SelectedUpgrade)
            utils::SetColor(Color4f(1.0f, 0.9f, 0.3f, 0.5f));
        else
            utils::SetColor(Color4f(0.2f, 0.2f, 0.2f, 0.3f));

        utils::FillRect(Rectf(slotX, slotY, slotW, slotH));

        m_AvailableUpgrades[i].Draw(
            menuLeft + i * (cardWidth + slotPadding),
            menuTop - cardHeight,
            cardWidth,
            cardHeight,
            false
        );
    }

    float cardsBottom = menuTop + slotPadding;
    float instructionY = cardsBottom + 20.f;
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 0.7f));
    Texture instr1("Use LEFT/RIGHT arrows and ENTER to select", "ShortBaby.ttf", 16, Color4f{ 1.0f, 1.0f, 1.0f, 0.7f });
    instr1.Draw(Vector2f(m_Width / 2.f - 150.f, instructionY));
    Texture instr2("Or click/double-click to choose upgrade", "ShortBaby.ttf", 16, Color4f{ 1.0f, 1.0f, 1.0f, 0.7f });
    instr2.Draw(Vector2f(m_Width / 2.f - 150.f, instructionY + 20.f));
}



bool Game::ProcessEnemyAttacks(float elapsedSec)
{
    bool towerDamaged = false;
    Rectf towerRect = m_pTower->GetPosition();

    for (EnemyBase* enemy : m_pEnemies)
    {
        if (enemy->GetType() == EnemyType::MELEE)
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

    if (m_GameState == GameState::UpgradeMenu && !m_AvailableUpgrades.empty())
    {
        if (m_SelectedUpgrade < m_AvailableUpgrades.size())
        {

            if (m_AvailableUpgrades[m_SelectedUpgrade].GetType() == UpgradeType::REPAIR)
            {

                UpdateTowerHealth(static_cast<int>(m_AvailableUpgrades[m_SelectedUpgrade].GetAmount()));
            }
            else
            {
                m_AvailableUpgrades[m_SelectedUpgrade].Apply(*m_pTower);
            }
        }
    }

    UpdateTowerHealth(50);
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
    {
        m_TowerHealth = m_MaxTowerHealth;
    }
}

void Game::GameOver() const
{
    utils::SetColor(Color4f(0.0f, 0.0f, 0.0f, 0.8f));
    utils::FillRect(Rectf(0, 0, m_Width, m_Height));

    utils::SetColor(Color4f(1.0f, 0.2f, 0.2f, 1.0f));
    Texture gameOverText("GAME OVER", "ShortBaby.ttf", 40, Color4f{ 1.0f, 0.2f, 0.2f, 1.0f });
    gameOverText.Draw(Vector2f(m_Width / 2.f - 100.f, m_Height / 2.f + 50.f));

    std::string waveText = "You reached wave " + std::to_string(m_CurrentWave);
    Texture waveReachedText(waveText, "ShortBaby.ttf", 24, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
    waveReachedText.Draw(Vector2f(m_Width / 2.f - 120.f, m_Height / 2.f));

    Texture restartText("Press ENTER or SPACE to restart", "ShortBaby.ttf", 20, Color4f{ 1.0f, 1.0f, 1.0f, 0.8f });
    restartText.Draw(Vector2f(m_Width / 2.f - 150.f, m_Height / 2.f - 50.f));
}