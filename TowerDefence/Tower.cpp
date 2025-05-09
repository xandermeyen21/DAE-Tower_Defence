#include "pch.h"
#include "Tower.h"
#include "Bullet.h"
#include "EnemyBase.h"
#include "utils.h"
#include <cmath>

Tower::Tower(Rectf tower, float range, float damage)
    : m_Range(range)
    , m_Damage(damage)
    , m_AttackSpeed(1.f) 
    , m_AttackTimer(0.f)
    , m_Tower(tower)
{
}

void Tower::Draw() const
{
    
    utils::SetColor(Color4f(0.f, 1.f, 1.f, 0.2f));
    utils::FillEllipse(Vector2f(m_Tower.left + m_Tower.width / 2, m_Tower.bottom + m_Tower.height / 2), m_Range, m_Range);

    
    utils::SetColor(Color4f(0.f, 0.7f, 1.f, 1.f)); 
    utils::FillRect(m_Tower);

    
    utils::SetColor(Color4f(0.f, 0.f, 0.5f, 1.f)); 
    utils::DrawRect(m_Tower);

    for (const Bullet& bullet : m_Bullets)
    {
        bullet.Draw();
    }
}

void Tower::Update(float elapsedSec, const std::vector<EnemyBase*>& enemies)
{
  
    for (size_t i = 0; i < m_Bullets.size(); ++i)
    {
        m_Bullets[i].Update(elapsedSec);
    }

    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());

    m_AttackTimer += elapsedSec;

    if (m_AttackTimer >= 1.f / m_AttackSpeed)
    {
        float towerCenterX = m_Tower.left + m_Tower.width / 2;
        float towerCenterY = m_Tower.bottom + m_Tower.height / 2;

        EnemyBase* closestEnemy = nullptr;
        float closestDistance = m_Range + 1.f; 

        for (EnemyBase* enemy : enemies)
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

       
        if (closestEnemy != nullptr)
        {
            const Ellipsef& enemyShape = closestEnemy->GetShape();
            m_Bullets.emplace_back(
                towerCenterX, towerCenterY,
                enemyShape.center.x, enemyShape.center.y,
                300.f, 
                static_cast<int>(m_Damage)
            );

            
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