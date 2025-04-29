#include "pch.h"
#include "Enemie.h"
#include "utils.h"
#include <cmath>

Enemie::Enemie(Ellipsef enemie, int hp, float WalkingSpeed)
    : m_WalkingSpeed(WalkingSpeed)
    , m_Hp(hp)
    , m_enemie(enemie)
    , m_IsAlive(true)
{
}

void Enemie::Draw() const
{
    if (m_IsAlive)
    {
       
        utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
        utils::FillEllipse(m_enemie);

        
        const float barWidth = m_enemie.radiusX * 2;
        const float barHeight = 5.f;
        const float barY = m_enemie.center.y + m_enemie.radiusY + 5.f;

        
        utils::SetColor(Color4f(0.3f, 0.3f, 0.3f, 1.f));
        utils::FillRect(Rectf(m_enemie.center.x - barWidth / 2, barY, barWidth, barHeight));

        
        utils::SetColor(Color4f(0.2f, 0.8f, 0.2f, 1.f));
        float healthPercentage = static_cast<float>(m_Hp) / 100.f; 
        float filledWidth = barWidth * healthPercentage;
        utils::FillRect(Rectf(m_enemie.center.x - barWidth / 2, barY, filledWidth, barHeight));
    }
}

void Enemie::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive)
        return;

  
    float dx = targetX - m_enemie.center.x;
    float dy = targetY - m_enemie.center.y;

 
    float distance = std::sqrt(dx * dx + dy * dy);

  
    if (distance > 0.1f)
    {
        dx /= distance;
        dy /= distance;
        m_enemie.center.x += dx * m_WalkingSpeed * elapsedSec;
        m_enemie.center.y += dy * m_WalkingSpeed * elapsedSec;
    }
}

bool Enemie::TakeDamage(int damage)
{
    if (!m_IsAlive)
        return false;

    m_Hp -= damage;

    if (m_Hp <= 0)
    {
        m_IsAlive = false;
        return true; 
    }

    return false;
}

bool Enemie::IsAlive() const
{
    return m_IsAlive;
}

const Ellipsef& Enemie::GetShape() const
{
    return m_enemie;
}

float Enemie::GetDistanceToTarget(float targetX, float targetY) const
{
    float dx = targetX - m_enemie.center.x;
    float dy = targetY - m_enemie.center.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool Enemie::HasReachedTarget(float targetX, float targetY) const
{
   
    return GetDistanceToTarget(targetX, targetY) < 5.0f;
}