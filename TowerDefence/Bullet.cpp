#include "pch.h"
#include "Bullet.h"
#include "utils.h"
#include <cmath>
#include "EnemyBase.h"

Bullet::Bullet(float x, float y, float targetX, float targetY, float speed, int damage, int hp)
    : m_Position{ x, y },
    m_Direction{ 0, 0 },
    m_Speed(speed),
    m_Damage(damage),
    m_IsActive(true),
    m_HP(hp)
{
    m_Position.x = x;
    m_Position.y = y;

    float dx = targetX - x;
    float dy = targetY - y;

    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0)
    {
        m_Direction.x = dx / length;
        m_Direction.y = dy / length;
    }
    else
    {
        m_Direction.x = 0;
        m_Direction.y = 0;
    }
}

void Bullet::Draw() const
{
    if (m_IsActive)
    {
       
        if (m_HP > 1)
        {
         
            utils::SetColor(Color4f(0.5f, 0.8f, 1.0f, 1.0f));
        }
        else
        {

            utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
        }
        utils::FillEllipse(m_Position, m_Radius, m_Radius);
    }
}

void Bullet::Update(float elapsedSec, float windowWidth, float windowHeight)
{
    if (m_IsActive)
    {
        m_Position.x += m_Direction.x * m_Speed * elapsedSec;
        m_Position.y += m_Direction.y * m_Speed * elapsedSec;
        if (m_Position.x < -50 || m_Position.x > windowWidth + 50 ||
            m_Position.y < -50 || m_Position.y > windowHeight + 50)
        {
            m_IsActive = false;
        }
    }
}

bool Bullet::IsActive() const
{
    return m_IsActive;
}

bool Bullet::CheckHit(const Ellipsef& target) const {
    if (!m_IsActive) return false;

    float dx = (m_Position.x - target.center.x) / target.radiusX;
    float dy = (m_Position.y - target.center.y) / target.radiusY;

    return (dx * dx + dy * dy) <= 1.0f;
}

int Bullet::GetDamage() const
{
    return m_Damage;
}

void Bullet::SetTarget(float targetX, float targetY)
{
    float dx = targetX - m_Position.x;
    float dy = targetY - m_Position.y;

    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0)
    {
        m_Direction.x = dx / length;
        m_Direction.y = dy / length;
    }
    else
    {
        m_Direction.x = 0;
        m_Direction.y = 0;
    }
}