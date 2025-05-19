#include "pch.h"
#include "EnemyBase.h"
#include "utils.h"
#include <cmath>

EnemyBase::EnemyBase(const Ellipsef& shape, int hp, float walkingSpeed, EnemyType type)
    : m_Shape(shape),
    m_Health(hp),
    m_MaxHealth(hp),
    m_Damage(1.0f),
    m_Speed(walkingSpeed),
    m_Target(shape.center),
    m_HasReachedTarget(false),
    m_Type(type)
{
    // No need to set position, radius values here - they're set in the initializer list
}

void EnemyBase::Update(float elapsedSec)
{
    if (!IsAlive()) return;

    if (!m_HasReachedTarget)
    {
        float dx = m_Target.x - m_Shape.center.x;
        float dy = m_Target.y - m_Shape.center.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < 1.0f)
        {
            m_HasReachedTarget = true;
            return;
        }

        float dirX = dx / distance;
        float dirY = dy / distance;
        float moveDistance = m_Speed * elapsedSec;

        if (moveDistance > distance)
        {
            m_Shape.center = m_Target;
            m_HasReachedTarget = true;
        }
        else
        {
            m_Shape.center.x += dirX * moveDistance;
            m_Shape.center.y += dirY * moveDistance;
        }
    }
}

void EnemyBase::Update(float targetX, float targetY, float elapsedSec)
{
    SetTarget(Vector2f(targetX, targetY));

    Update(elapsedSec);
}

void EnemyBase::Draw() const
{
    utils::SetColor(GetDrawColor());  
    utils::FillEllipse(m_Shape.center, m_Shape.radiusX, m_Shape.radiusY);

    // Draw health bar above the enemy
    const float healthBarWidth = 30.0f;
    const float healthBarHeight = 5.0f;
    const float healthBarY = m_Shape.center.y + m_Shape.radiusY + 5.0f;
    const float healthBarX = m_Shape.center.x - healthBarWidth / 2.0f;

    utils::SetColor(Color4f(0.3f, 0.3f, 0.3f, 0.7f));
    utils::FillRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight);

    float healthPercentage = m_Health / m_MaxHealth;
    utils::SetColor(Color4f(0.2f, 0.8f, 0.2f, 0.7f));
    utils::FillRect(healthBarX, healthBarY, healthBarWidth * healthPercentage, healthBarHeight);
}

const Ellipsef& EnemyBase::GetShape() const { return m_Shape; }
float EnemyBase::GetHealth() const { return m_Health; }
float EnemyBase::GetMaxHealth() const { return m_MaxHealth; }
float EnemyBase::GetDamage() const { return m_Damage; }
float EnemyBase::GetSpeed() const { return m_Speed; }
bool EnemyBase::IsAlive() const { return m_Health > 0.0f; }

void EnemyBase::TakeDamage(float damage)
{
    m_Health -= damage;
    if (m_Health < 0.0f) m_Health = 0.0f;
}

void EnemyBase::SetPosition(const Vector2f& position)
{
    m_Shape.center = position;
}

void EnemyBase::SetTarget(const Vector2f& target)
{
    m_Target = target;
    m_HasReachedTarget = false;
}

bool EnemyBase::HasReachedTarget() const
{
    return m_HasReachedTarget;
}
