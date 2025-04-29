#include "pch.h"
#include "Game.h"
#include "Tower.h"
#include "Enemie.h"
#include "Bullet.h"
#include "utils.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include "Texture.h"

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
    , m_SelectedUpgrade{ 0 }
    , m_Width{ window.width }
    , m_Height{ window.height }
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
}

void Game::Cleanup()
{
    delete m_pTower;

   
    for (Enemie* enemy : m_pEnemies)
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
        
        m_pTower->Update(elapsedSec, m_pEnemies);

        
        if (m_WaveInProgress)
        {
            m_EnemySpawnTimer += elapsedSec;
            if (m_EnemySpawnTimer >= m_EnemySpawnInterval && m_pEnemies.size() < m_MaxEnemies &&
                m_EnemiesKilled + m_pEnemies.size() < m_EnemiesRequiredForWave)
            {
                SpawnEnemy();
                m_EnemySpawnTimer = 0.f;
            }
        }

        
        for (size_t i = 0; i < m_pEnemies.size();)
        {
           
            float towerCenterX = m_pTower->GetPosition().left + m_pTower->GetPosition().width / 2.f;
            float towerCenterY = m_pTower->GetPosition().bottom + m_pTower->GetPosition().height / 2.f;

            
            m_pEnemies[i]->Update(towerCenterX, towerCenterY, elapsedSec);

            
            if (m_pEnemies[i]->HasReachedTarget(towerCenterX, towerCenterY))
            {
                
                m_GameState = GameState::GameOver;
                return;
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
        break;

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

    
    for (const Enemie* enemy : m_pEnemies)
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

void Game::SpawnEnemy()
{
    
    float spawnX, spawnY;
    float radius = 15.f; // Enemy radius

  
    int edge = rand() % 4;

    switch (edge)
    {
    case 0: // Top edge
        spawnX = float(rand() % int(m_Width));
        spawnY = m_Height + radius;
        break;
    case 1: // Right edge
        spawnX = m_Width + radius;
        spawnY = float(rand() % int(m_Height));
        break;
    case 2: // Bottom edge
        spawnX = float(rand() % int(m_Width));
        spawnY = -radius;
        break;
    case 3: // Left edge
        spawnX = -radius;
        spawnY = float(rand() % int(m_Height));
        break;
    }

    
    int hp = 1;
    
    float speed = 40.f + (m_CurrentWave * 5);
    Enemie* enemy = new Enemie(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);
    m_pEnemies.push_back(enemy);
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
           
            UpgradeOption& option = m_UpgradeOptions[m_SelectedUpgrade];
            (m_pTower->*(option.upgradeFunction))(option.upgradeAmount);

            
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
            m_WaveInProgress = true;
            m_GameState = GameState::Playing;

            
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
                    
                    UpgradeOption& option = m_UpgradeOptions[m_SelectedUpgrade];
                    (m_pTower->*(option.upgradeFunction))(option.upgradeAmount);

                    
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

bool Game::ProcessBulletCollisions(Enemie* enemy)
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
    if (m_WaveInProgress && m_EnemiesKilled >= m_EnemiesRequiredForWave && m_pEnemies.empty())
    {
        m_WaveInProgress = false;
        m_GameState = GameState::UpgradeMenu;
    }
}

void Game::StartNextWave()
{
  
    m_CurrentWave++;

   
    m_EnemiesKilled = 0;


    m_EnemiesRequiredForWave = 5 + (m_CurrentWave * 2);

    
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