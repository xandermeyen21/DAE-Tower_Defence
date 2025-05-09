#pragma once
#include "EnemyBase.h"

class MeleeEnemy : public EnemyBase
{
public:
    MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed);
    virtual void Draw() const override;
    virtual void Update(float targetX, float targetY, float elapsedSec) override;
    virtual bool Attack(float elapsedSec, const Rectf& towerShape) override;
    virtual bool CanAttack(float targetX, float targetY, float elapsedSec) const override;
    virtual int GetAttackDamage() const override;
    bool HasReachedTarget(float targetX, float targetY, float distance) const;
    float GetPreferredDistance() const;

    friend class Game;

private:
    float m_PreferredDistance; 
    float m_AttackCooldown;    
    bool m_AtTower;            

    
};