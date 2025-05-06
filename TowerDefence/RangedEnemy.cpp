#include "pch.h"
#include "RangedEnemy.h"
#include "utils.h"

RangedEnemy::RangedEnemy(Ellipsef shape, int hp, float walkingSpeed)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::RANGED)
    , m_AttackTimer(0.f)
    , m_AttackSpeed(0.5f) 
    , m_BulletDamage(1.f)
{
    m_PreferredDistance = 150.0f; 
}

void RangedEnemy::Draw() const
{
    if (!m_IsAlive) return;

    
    utils::SetColor(Color4f(0.2f, 0.2f, 1.f, 1.f));
    utils::FillEllipse(m_Shape);

    
    utils::SetColor(Color4f(0.f, 0.5f, 1.f, 1.f));
    utils::FillTriangle(
        Vector2f(m_Shape.center.x, m_Shape.center.y + m_Shape.radiusY * 1.5f),
        Vector2f(m_Shape.center.x - m_Shape.radiusX, m_Shape.center.y),
        Vector2f(m_Shape.center.x + m_Shape.radiusX, m_Shape.center.y)
    );

    
    DrawHealthBar();

    // Draw bullets
    for (const Bullet& bullet : m_Bullets)
    {
        bullet.Draw();
    }
}

void RangedEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;

   
    EnemyBase::Update(targetX, targetY, elapsedSec);

    
    for (size_t i = 0; i < m_Bullets.size(); ++i)
    {
        m_Bullets[i].Update(elapsedSec);
    }

    
    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());
}

bool RangedEnemy::Attack(float elapsedSec, Rectf& towerShape)
{
    if (!m_IsAlive) return false;

    float towerCenterX = towerShape.left + towerShape.width / 2.f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.f;

    float distance = GetDistanceToTarget(towerCenterX, towerCenterY);

    
    if (distance <= m_PreferredDistance * 1.2f) 
    {
        m_AttackTimer += elapsedSec;

        if (m_AttackTimer >= 1.f / m_AttackSpeed)
        {
            
            m_Bullets.emplace_back(
                m_Shape.center.x, m_Shape.center.y,
                towerCenterX, towerCenterY,
                200.f, 
                static_cast<int>(m_BulletDamage)
            );

            
            m_AttackTimer = 0.f;
            return true;
        }
    }

    return false;
}

const std::vector<Bullet>& RangedEnemy::GetBullets() const
{
    return m_Bullets;
}

std::vector<Bullet>& RangedEnemy::GetBullets()
{
    return m_Bullets;
}
