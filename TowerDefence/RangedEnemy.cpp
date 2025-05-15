#include "pch.h"
#include "RangedEnemy.h"
#include "Tower.h"
#include "utils.h"
#include <cmath>
#include <algorithm>

// Fixed: Changed constructor signature to match EnemyBase
RangedEnemy::RangedEnemy(const Vector2f& position, float health, float damage, float speed)
    : EnemyBase(Ellipsef{ position, 15.0f, 15.0f }, static_cast<int>(health), speed, EnemyType::Ranged)
    , m_IsShooting(false)
    , m_AttackTimer(0.0f)
    , m_AttackSpeed(1.0f)
    , m_AttackRange(150.0f)
    , m_BulletDamage(damage)
    , m_ShootCooldownMax(1.5f)
    , m_ShootCooldown(0.0f)
{
}

// Fixed: Changed constructor signature to match EnemyBase
RangedEnemy::RangedEnemy(const Ellipsef& shape, float health, float speed)
    : EnemyBase(shape, static_cast<int>(health), speed, EnemyType::Ranged)
    , m_IsShooting(false)
    , m_AttackTimer(0.0f)
    , m_AttackSpeed(1.0f)
    , m_AttackRange(150.0f)
    , m_BulletDamage(10.0f)
    , m_ShootCooldownMax(1.5f)
    , m_ShootCooldown(0.0f)
{
}

void RangedEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!IsAlive()) return;

    // Fixed: Call the base class Update method correctly
    EnemyBase::Update(targetX, targetY, elapsedSec);

    m_IsShooting = false;
    if (m_AttackTimer > 0)
        m_AttackTimer -= elapsedSec;
    if (m_ShootCooldown > 0)
        m_ShootCooldown -= elapsedSec;

    float enemyX = GetShape().center.x;
    float enemyY = GetShape().center.y;
    float dx = targetX - enemyX;
    float dy = targetY - enemyY;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= m_AttackRange && m_AttackTimer <= 0)
    {
        m_IsShooting = true;
        m_AttackTimer = 1.0f / m_AttackSpeed;
        ShootIfAble(targetX, targetY, elapsedSec);
    }

    UpdateBullets(elapsedSec);
}

void RangedEnemy::Draw() const
{
    EnemyBase::Draw();
    if (m_IsShooting)
    {
        utils::SetColor(Color4f(1.0f, 0.0f, 0.0f, 0.7f));
        utils::FillEllipse(GetShape().center, GetShape().radiusX * 1.2f, GetShape().radiusY * 1.2f);
    }

    utils::SetColor(Color4f(0.8f, 0.2f, 0.2f, 0.1f));
    utils::FillEllipse(GetShape().center, m_AttackRange, m_AttackRange);

    for (const Bullet& bullet : m_Bullets)
    {
        if (bullet.IsActive())
            bullet.Draw();
    }
}

// Fixed: Implementation to match the declaration in the header
void RangedEnemy::Update(float elapsedSec, const std::vector<Tower*>& towers)
{
    if (!IsAlive()) return;

    if (towers.empty()) {
        EnemyBase::Update(elapsedSec);
        return;
    }

    Tower* closestTower = nullptr;
    float closestDistance = std::numeric_limits<float>::max();
    float towerX = 0.0f;
    float towerY = 0.0f;

    for (Tower* tower : towers)
    {
        const Rectf& towerPos = tower->GetPosition();
        float tempX = towerPos.left + towerPos.width / 2.0f;
        float tempY = towerPos.bottom + towerPos.height / 2.0f;

        float dx = tempX - m_Shape.center.x;
        float dy = tempY - m_Shape.center.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestTower = tower;
            towerX = tempX;
            towerY = tempY;
        }
    }

    if (closestTower != nullptr)
    {
        Update(towerX, towerY, elapsedSec);
    }
    else
    {
        EnemyBase::Update(elapsedSec);
    }
}

bool RangedEnemy::Attack(float elapsedSec, const Rectf& towerShape)
{
    if (!IsAlive() || m_ShootCooldown > 0.0f) return false;

    float towerCenterX = towerShape.left + towerShape.width / 2.0f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.0f;

    float enemyX = GetShape().center.x;
    float enemyY = GetShape().center.y;

    float dx = towerCenterX - enemyX;
    float dy = towerCenterY - enemyY;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= m_AttackRange) {
        ShootIfAble(towerCenterX, towerCenterY, elapsedSec);
        return true;
    }

    return false;
}

void RangedEnemy::ShootIfAble(float targetX, float targetY, float elapsedSec)
{
    if (m_ShootCooldown <= 0.0f)
    {
        float enemyX = GetShape().center.x;
        float enemyY = GetShape().center.y;
        m_Bullets.emplace_back(
            enemyX, enemyY,
            targetX, targetY,
            200.0f,
            m_BulletDamage,
            0
        );
        m_ShootCooldown = m_ShootCooldownMax;
    }
}

void RangedEnemy::UpdateBullets(float elapsedSec)
{
    for (Bullet& bullet : m_Bullets)
    {
        if (bullet.IsActive())
            bullet.Update(elapsedSec);
    }
    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());
}

std::vector<Bullet>& RangedEnemy::GetBullets()
{
    return m_Bullets;
}

const std::vector<Bullet>& RangedEnemy::GetBullets() const
{
    return m_Bullets;
}

bool RangedEnemy::IsShooting() const
{
    return m_IsShooting;
}

float RangedEnemy::GetAttackRange() const
{
    return m_AttackRange;
}

int RangedEnemy::GetAttackDamage() const
{
    return static_cast<int>(m_BulletDamage);
}

bool RangedEnemy::CanAttack(float targetX, float targetY, float elapsedSec) const
{
    if (!IsAlive() || m_ShootCooldown > 0.0f) return false;

    float enemyX = GetShape().center.x;
    float enemyY = GetShape().center.y;
    float dx = targetX - enemyX;
    float dy = targetY - enemyY;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance <= m_AttackRange;
}