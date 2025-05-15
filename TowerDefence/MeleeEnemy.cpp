#include "pch.h"
#include "MeleeEnemy.h"
#include "utils.h"
#include <cmath>

MeleeEnemy::MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::Melee),  
    m_AttackCooldown(0.0f),
    m_AtTower(false),
    m_PowerLevel(1),
    m_PreferredDistance(20.0f)
{
    // Move initialization to initializer list
    int baseHp = 1 + (hp / (1 + 0.3f));
    if (hp > baseHp * 1.5f) {
        m_PowerLevel = 2;
    }
    else if (hp > baseHp * 2.2f) {
        m_PowerLevel = 3;
    }
    else if (hp > baseHp * 3.0f) {
        m_PowerLevel = 4;
    }
}

void MeleeEnemy::Draw() const
{
    if (!IsAlive()) return;  // Fixed: m_IsAlive to IsAlive()

    // Base enemy color
    utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
    utils::FillEllipse(m_Shape);

    if (m_PowerLevel > 1) {
        float pulseIntensity = 0.6f + 0.2f * sin(SDL_GetTicks() * 0.005f);
        Color4f powerColor;
        switch (m_PowerLevel) {
        case 2:
            powerColor = Color4f(1.0f, 0.7f, 0.0f, pulseIntensity);
            break;
        case 3:
            powerColor = Color4f(1.0f, 0.4f, 0.0f, pulseIntensity);
            break;
        default:
            powerColor = Color4f(1.0f, 0.0f, 0.0f, pulseIntensity);
            break;
        }
        utils::SetColor(powerColor);
        utils::FillEllipse(m_Shape.center, m_Shape.radiusX * 1.2f, m_Shape.radiusY * 1.2f);
        utils::SetColor(Color4f(1.f, 0.2f, 0.3f, 1.f));
        utils::FillEllipse(m_Shape);
    }

    DrawHealthBar();
}

void MeleeEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!IsAlive()) return;

    // Call the correct base class Update that handles setting the target and basic movement
    EnemyBase::Update(targetX, targetY, elapsedSec);

    if (m_AttackCooldown > 0.0f) {
        m_AttackCooldown -= elapsedSec;
    }

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Fine-tuning of position to maintain preferred distance
    if (distance <= m_PreferredDistance - 0.5f && distance > 0.1f) {
        dx /= distance;
        dy /= distance;
        m_Shape.center.x -= dx * m_Speed * 0.5f * elapsedSec;
        m_Shape.center.y -= dy * m_Speed * 0.5f * elapsedSec;
    }

    m_AtTower = (distance <= m_PreferredDistance + 5.0f);
}


// Add DrawHealthBar implementation
void MeleeEnemy::DrawHealthBar() const
{
    const float healthBarWidth = 30.0f;
    const float healthBarHeight = 5.0f;
    const float healthBarY = m_Shape.center.y + m_Shape.radiusY + 5.0f;
    const float healthBarX = m_Shape.center.x - healthBarWidth / 2.0f;

    utils::SetColor(Color4f(0.3f, 0.3f, 0.3f, 0.7f));
    utils::FillRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight);

    float healthPercentage = GetHealth() / GetMaxHealth();
    utils::SetColor(Color4f(0.2f, 0.8f, 0.2f, 0.7f));
    utils::FillRect(healthBarX, healthBarY, healthBarWidth * healthPercentage, healthBarHeight);
}

bool MeleeEnemy::Attack(float elapsedSec, const Rectf& towerShape)
{
    if (!IsAlive() || m_AttackCooldown > 0.0f) return false;

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
    return IsAlive() && m_AttackCooldown <= 0.0f && HasReachedTarget(targetX, targetY, m_PreferredDistance);
}

int MeleeEnemy::GetAttackDamage() const
{
    return 1 * m_PowerLevel;
}

float MeleeEnemy::GetPreferredDistance() const
{
    return m_PreferredDistance;
}

bool MeleeEnemy::HasReachedTarget(float targetX, float targetY, float distance) const
{
    if (!IsAlive()) return false;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float actualDistance = std::sqrt(dx * dx + dy * dy);

    return actualDistance <= distance + 5.0f;
}

// Add missing implementation of pure virtual function
EnemyType MeleeEnemy::GetType() const
{
    return EnemyType::Melee;
}
