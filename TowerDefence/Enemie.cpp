#include "pch.h"
#include "Enemie.h"
#include "utils.h"
#include <cmath>

Enemie::Enemie(Ellipsef enemie, int hp, float WalkingSpeed)
    :m_WalkingSpeed{ WalkingSpeed }
    , m_Hp{ hp }
    , m_enemie{ enemie }
{
}

void Enemie::Draw() const // Made Draw() const
{
    utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
    utils::FillEllipse(m_enemie);
}

void Enemie::Update(float targetX, float targetY, float elapsedSec)
{
    // Calculate direction vector to tower
    float dx = targetX - m_enemie.center.x;
    float dy = targetY - m_enemie.center.y;

    // Normalize the direction vector
    float distance = std::sqrt(dx * dx + dy * dy);

    // Only move if we're not already at the target
    if (distance > 0.1f)
    {
        dx /= distance;
        dy /= distance;

        // Move enemy toward tower
        m_enemie.center.x += dx * m_WalkingSpeed * elapsedSec;
        m_enemie.center.y += dy * m_WalkingSpeed * elapsedSec;
    }
}