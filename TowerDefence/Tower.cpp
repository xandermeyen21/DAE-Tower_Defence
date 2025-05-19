#include "pch.h"
#include "Tower.h"
#include "Bullet.h"
#include "utils.h"
#include "EnemyBase.h" 
#include <vector>
#include <cmath>
#include <algorithm>

Tower::Tower(Rectf tower, float range, float damage)
    : m_Tower(tower)
    , m_Range(range)
    , m_Damage(damage)
    , m_AttackSpeed(1.0f)
    , m_AttackTimer(0.0f)
    , m_MaxHealth(100)
    , m_Health(100)
    , m_RicochetCount(0)
{
}

void Tower::Update(float elapsedSec, const std::vector<EnemyBase*>& enemies)
{
    if (m_AttackTimer > 0.0f)
    {
        m_AttackTimer -= elapsedSec;
    }

    for (auto bulletIt = m_Bullets.begin(); bulletIt != m_Bullets.end();)
    {
        bulletIt->Update(elapsedSec);
        if (!bulletIt->IsActive())
        {
            bulletIt = m_Bullets.erase(bulletIt);
        }
        else
        {
            ++bulletIt;
        }
    }

    if (m_AttackTimer <= 0.0f && !enemies.empty())
    {
        float towerCenterX = m_Tower.left + m_Tower.width / 2.0f;
        float towerCenterY = m_Tower.bottom + m_Tower.height / 2.0f;

        EnemyBase* nearestEnemy = nullptr;
        float nearestDistance = m_Range + 1.0f;
        for (EnemyBase* enemy : enemies)
        {
            float enemyCenterX = enemy->GetShape().center.x;
            float enemyCenterY = enemy->GetShape().center.y;

            float dx = enemyCenterX - towerCenterX;
            float dy = enemyCenterY - towerCenterY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= m_Range && distance < nearestDistance)
            {
                nearestDistance = distance;
                nearestEnemy = enemy;
            }
        }

        if (nearestEnemy)
        {
            float enemyCenterX = nearestEnemy->GetShape().center.x;
            float enemyCenterY = nearestEnemy->GetShape().center.y;
            int bulletHP = 1 + m_RicochetCount;

            m_Bullets.emplace_back(
                towerCenterX, towerCenterY,
                enemyCenterX, enemyCenterY,
                200.0f, 
                m_Damage,
                bulletHP
            );

            m_AttackTimer = 1.0f / m_AttackSpeed; 
        }
    }
}

void Tower::Draw() const
{
    utils::SetColor(Color4f(0.2f, 0.2f, 0.8f, 1.0f));
    utils::FillRect(m_Tower);

    utils::SetColor(Color4f(0.5f, 0.5f, 0.9f, 0.2f));
    float centerX = m_Tower.left + m_Tower.width / 2.0f;
    float centerY = m_Tower.bottom + m_Tower.height / 2.0f;
    utils::FillEllipse(centerX, centerY, m_Range, m_Range);

    for (const Bullet& bullet : m_Bullets)
    {
        bullet.Draw();
    }
}

const Rectf& Tower::GetPosition() const
{
    return m_Tower;
}

float Tower::GetRange() const
{
    return m_Range;
}

float Tower::GetDamage() const
{
    return m_Damage;
}

float Tower::GetAttackSpeed() const
{
    return m_AttackSpeed;
}

std::vector<Bullet>& Tower::GetBullets()
{
    return m_Bullets;
}

void Tower::UpgradeDamage(float amount)
{
    m_Damage += amount;
}

void Tower::IncreaseDamage(float amount)
{
    UpgradeDamage(amount);
}

void Tower::UpgradeAttackSpeed(float amount)
{
    m_AttackSpeed += amount;
}

void Tower::IncreaseAttackSpeed(float amount)
{
    UpgradeAttackSpeed(amount);
}

void Tower::UpgradeRange(float amount)
{
    m_Range += amount;
}

void Tower::IncreaseRange(float amount)
{
    UpgradeRange(amount);
}

void Tower::UpgradeRicochet(int amount)
{
    m_RicochetCount += amount;
}

void Tower::IncreaseRicochet(int amount)
{
    UpgradeRicochet(static_cast<int>(amount));
}

void Tower::UpgradeMaxHealth(float amt)
{
    m_MaxHealth += static_cast<int>(amt);
    m_Health += static_cast<int>(amt);
}

int Tower::GetRicochetCount() const
{
    return m_RicochetCount;
}

void Tower::ResetToInitialStats()
{
    m_Damage = 1.f;
    m_Range = 150.f;
    m_AttackSpeed = 1.f;
    m_MaxHealth = 100;
    m_Health = 100;
    m_RicochetCount = 0;
    
}

void Tower::ClearBullets()
{
    m_Bullets.clear();
}