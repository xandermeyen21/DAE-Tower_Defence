#include "pch.h"
#include "Tower.h"
#include "Enemie.h"
#include "utils.h"
#include <cmath>

Tower::Tower(Rectf tower, float range, float damage)
    : m_Range(range)
    , m_Damage(damage)
    , m_AttackSpeed(1.f) // 1 attack per second default
    , m_AttackTimer(0.f)
    , m_Tower(tower)
{
}

void Tower::Draw() const
{
    // Draw tower
    utils::SetColor(Color4f(0.f, 1.f, 1.f, 1.f));
    utils::FillRect(m_Tower);

    // Draw range indicator (semi-transparent circle)
    utils::SetColor(Color4f(0.f, 1.f, 1.f, 0.2f));
    utils::FillEllipse(Vector2f(m_Tower.left + m_Tower.width / 2, m_Tower.bottom + m_Tower.height / 2), m_Range, m_Range);

    // Draw bullets
    for (const Bullet& bullet : m_Bullets)
    {
        bullet.Draw();
    }
}

void Tower::Update(float elapsedSec, const std::vector<Enemie*>& enemies)
{
    // Update all bullets
    for (size_t i = 0; i < m_Bullets.size(); ++i)
    {
        m_Bullets[i].Update(elapsedSec);
    }

    // Remove inactive bullets
    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());

    // Update attack timer
    m_AttackTimer += elapsedSec;

    // Check if we can attack
    if (m_AttackTimer >= 1.f / m_AttackSpeed)
    {
        // Find closest enemy in range
        float towerCenterX = m_Tower.left + m_Tower.width / 2;
        float towerCenterY = m_Tower.bottom + m_Tower.height / 2;

        Enemie* closestEnemy = nullptr;
        float closestDistance = m_Range + 1.f; // Initialize to something larger than range

        for (Enemie* enemy : enemies)
        {
            const Ellipsef& enemyShape = enemy->GetShape();
            float dx = enemyShape.center.x - towerCenterX;
            float dy = enemyShape.center.y - towerCenterY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= m_Range && distance < closestDistance)
            {
                closestDistance = distance;
                closestEnemy = enemy;
            }
        }

        // If we found an enemy in range, shoot at it
        if (closestEnemy != nullptr)
        {
            const Ellipsef& enemyShape = closestEnemy->GetShape();
            m_Bullets.emplace_back(
                towerCenterX, towerCenterY,
                enemyShape.center.x, enemyShape.center.y,
                300.f, // Bullet speed
                static_cast<int>(m_Damage)
            );

            // Reset attack timer
            m_AttackTimer = 0.f;
        }
    }
}

const Rectf& Tower::GetPosition() const
{
    return m_Tower;
}

void Tower::UpgradeDamage(float amount)
{
    m_Damage += amount;
}

void Tower::UpgradeRange(float amount)
{
    m_Range += amount;
}

void Tower::UpgradeAttackSpeed(float amount)
{
    m_AttackSpeed += amount;
}

float Tower::GetDamage() const
{
    return m_Damage;
}

float Tower::GetRange() const
{
    return m_Range;
}

float Tower::GetAttackSpeed() const
{
    return m_AttackSpeed;
}

std::vector<Bullet>& Tower::GetBullets()
{
    return m_Bullets;
}