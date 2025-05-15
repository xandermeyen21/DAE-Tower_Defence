#pragma once
#include "EnemyBase.h"

class MeleeEnemy : public EnemyBase
{
public:
    MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed);

    void Draw() const override;
    void Update(float targetX, float targetY, float elapsedSec) override;
    bool Attack(float elapsedSec, const Rectf& towerShape) override;
    bool CanAttack(float targetX, float targetY, float elapsedSec) const override;
    int GetAttackDamage() const override;
    EnemyType GetType() const override;

    float GetPreferredDistance() const;
    bool HasReachedTarget(float targetX, float targetY, float distance) const;

    float m_AttackCooldown;
    bool m_AtTower;

private:
    void DrawHealthBar() const;
    int m_PowerLevel;
    float m_PreferredDistance;
};
