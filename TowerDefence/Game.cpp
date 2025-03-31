#include "pch.h"
#include "Game.h"
#include "Tower.h"
#include "Enemie.h"
#include "utils.h"
#include <cmath>

Game::Game(const Window& window)
    : BaseGame{ window }
    , m_pTower{ nullptr }
    , m_EnemySpawnTimer{ 0.f }
    , m_EnemySpawnInterval{ 2.f } // Spawn an enemy every 2 seconds
    , m_MaxEnemies{ 10 }
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
    // Create tower at the center of the screen
    float towerWidth = 30.f;
    float towerHeight = 50.f;
    float centerX = m_Width / 2.f - towerWidth / 2.f;
    float centerY = m_Height / 2.f - towerHeight / 2.f;
    m_pTower = new Tower{ Rectf{centerX, centerY, towerWidth, towerHeight}, 150.f };
}

void Game::Cleanup()
{
    delete m_pTower;

    // Clean up all enemies
    for (Enemie* enemy : m_pEnemies)
    {
        delete enemy;
    }
    m_pEnemies.clear();
}

void Game::Update(float elapsedSec)
{
    // Update tower
    m_pTower->Update();

    // Enemy spawning logic
    m_EnemySpawnTimer += elapsedSec;
    if (m_EnemySpawnTimer >= m_EnemySpawnInterval && m_pEnemies.size() < m_MaxEnemies)
    {
        SpawnEnemy();
        m_EnemySpawnTimer = 0.f;
    }

    // Update all enemies and have them move toward the tower
    for (size_t i = 0; i < m_pEnemies.size(); i++)
    {
        // Get tower center position
        float towerCenterX = m_pTower->GetPosition().left + m_pTower->GetPosition().width / 2.f;
        float towerCenterY = m_pTower->GetPosition().bottom + m_pTower->GetPosition().height / 2.f;

        // Update enemy with tower position for movement
        m_pEnemies[i]->Update(towerCenterX, towerCenterY, elapsedSec);
    }
}

void Game::Draw() const
{
    ClearBackground();

    // Draw tower
    m_pTower->Draw();

    // Draw all enemies
    for (const Enemie* enemy : m_pEnemies)
    {
        enemy->Draw();
    }
}

void Game::SpawnEnemy()
{
    // Random position around the edges of the window
    float spawnX, spawnY;
    float radius = 15.f; // Enemy radius

    // Decide which edge to spawn from
    int edge = rand() % 4; // 0: top, 1: right, 2: bottom, 3: left

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

    // Create enemy
    int hp = 100;
    float speed = 40.f + (rand() % 30); // Random speed between 40 and 70
    Enemie* enemy = new Enemie(Ellipsef(spawnX, spawnY, radius, radius), hp, speed);
    m_pEnemies.push_back(enemy);
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
    // Add key handling if needed
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
    // Add mouse click handling if needed
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