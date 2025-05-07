#include "pch.h"
#include "EnemyBase.h"
#include "utils.h"
#include <cmath>

EnemyBase::EnemyBase(Ellipsef shape, int hp, float walkingSpeed, EnemyType type)
    : m_WalkingSpeed(walkingSpeed)
    , m_Hp(hp)
    , m_MaxHp(hp)
    , m_Shape(shape)
    , m_IsAlive(true)
    , m_Type(type)
    , m_PreferredDistance(5.0f) 
{
}

void EnemyBase::Draw() const
{
    if (!m_IsAlive) return;

    
    utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f)); 
    utils::FillEllipse(m_Shape);

    
    DrawHealthBar();
}

void EnemyBase::DrawHealthBar() const
{
    const float barWidth = m_Shape.radiusX * 2;
    const float barHeight = 5.f;
    const float barY = m_Shape.center.y + m_Shape.radiusY + 5.f;

    
    utils::SetColor(Color4f(0.3f, 0.3f, 0.3f, 1.f));
    utils::FillRect(Rectf(m_Shape.center.x - barWidth / 2, barY, barWidth, barHeight));

    
    utils::SetColor(Color4f(0.2f, 0.8f, 0.2f, 1.f));
    float healthPercentage = static_cast<float>(m_Hp) / m_MaxHp;
    float filledWidth = barWidth * healthPercentage;
    utils::FillRect(Rectf(m_Shape.center.x - barWidth / 2, barY, filledWidth, barHeight));
}

void EnemyBase::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    
    if (distance > m_PreferredDistance)
    {
       
        if (distance > 0.1f)
        {
            dx /= distance;
            dy /= distance;
            m_Shape.center.x += dx * m_WalkingSpeed * elapsedSec;
            m_Shape.center.y += dy * m_WalkingSpeed * elapsedSec;
        }
    }
}

bool EnemyBase::Attack(float elapsedSec, Rectf& towerShape)
{
    return false;
}

bool EnemyBase::TakeDamage(int damage)
{
    if (!m_IsAlive) return false;

    m_Hp -= damage;

    if (m_Hp <= 0)
    {
        m_IsAlive = false;
        return true; 
    }

    return false;
}

bool EnemyBase::IsAlive() const
{
    return m_IsAlive;
}

const Ellipsef& EnemyBase::GetShape() const
{
    return m_Shape;
}

float EnemyBase::GetDistanceToTarget(float targetX, float targetY) const
{
    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool EnemyBase::HasReachedTarget(float targetX, float targetY, float minDistance) const
{
    return GetDistanceToTarget(targetX, targetY) < minDistance;
}

EnemyType EnemyBase::GetType() const
{
    return m_Type;
}

int EnemyBase::GetHp() const
{
    return m_Hp;
}

int EnemyBase::GetMaxHp() const
{
    return m_MaxHp;
}

