#include "pch.h"
#include "MeleeEnemy.h"
#include "utils.h"
#include <iostream>

MeleeEnemy::MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::MELEE),
    m_AttackCooldown(0.0f),   
    m_AtTower(false)
{
    m_PreferredDistance = 20.0f;
}

void MeleeEnemy::Draw() const
{
    if (!m_IsAlive) return;
    utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
    utils::FillEllipse(m_Shape);
    DrawHealthBar();
}

void MeleeEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;

    if (m_AttackCooldown > 0.0f) {
        m_AttackCooldown -= elapsedSec;
    }

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance > m_PreferredDistance + 0.5f) {
        m_AtTower = false;
        if (distance > 0.1f) {
            dx /= distance;
            dy /= distance;
            m_Shape.center.x += dx * m_WalkingSpeed * elapsedSec;
            m_Shape.center.y += dy * m_WalkingSpeed * elapsedSec;
        }
    }
    else if (distance < m_PreferredDistance - 0.5f) {
        if (distance > 0.1f) {
            dx /= distance;
            dy /= distance;
            m_Shape.center.x -= dx * m_WalkingSpeed * 0.5f * elapsedSec;
            m_Shape.center.y -= dy * m_WalkingSpeed * 0.5f * elapsedSec;
        }
    }
    else {
        m_AtTower = true;
    }
}

bool MeleeEnemy::Attack(float elapsedSec, const Rectf& towerShape)
{
    if (!m_IsAlive || m_AttackCooldown > 0.0f) return false;

    float towerCenterX = towerShape.left + towerShape.width / 2.f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.f;

    if (HasReachedTarget(towerCenterX, towerCenterY, m_PreferredDistance)) {
        m_AttackCooldown = 1.0f; 
        return true;
    }
    return false;
}

bool MeleeEnemy::CanAttack(float targetX, float targetY, float elapsedSec) const
{
    return m_IsAlive && m_AttackCooldown <= 0.0f && HasReachedTarget(targetX, targetY, m_PreferredDistance);
}

int MeleeEnemy::GetAttackDamage() const
{
    return 1; 
}

float MeleeEnemy::GetPreferredDistance() const
{
    return m_PreferredDistance;
}

bool MeleeEnemy::HasReachedTarget(float targetX, float targetY, float distance) const
{
    if (!m_IsAlive) return false;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float actualDistance = std::sqrt(dx * dx + dy * dy);

    return actualDistance <= distance + 5.0f; 
}