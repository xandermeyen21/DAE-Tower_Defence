#include "pch.h"
#include "MeleeEnemy.h"
#include "utils.h"

MeleeEnemy::MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::MELEE)
{
    m_PreferredDistance = 5.0f; 
}

void MeleeEnemy::Draw() const 
{
    if (!m_IsAlive) return;

    
    utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
    utils::FillEllipse(m_Shape);

   
    DrawHealthBar();
}

bool MeleeEnemy::Attack(float elapsedSec, Rectf& towerShape)
{
    
    float towerCenterX = towerShape.left + towerShape.width / 2.f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.f;

    if (HasReachedTarget(towerCenterX, towerCenterY, m_PreferredDistance))
    {
        
        return true;
    }

    return false;
}