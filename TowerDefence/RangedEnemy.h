#pragma once
#include "EnemyBase.h"
#include "Bullet.h"
#include <vector>

class RangedEnemy : public EnemyBase {
public:
    RangedEnemy(Ellipsef shape, int hp, float walkingSpeed);
    void Draw() const override;
    void Update(float targetX, float targetY, float elapsedSec) override;
    bool Attack(float elapsedSec, const Rectf& towerShape) override;
    bool CanAttack(float targetX, float targetY, float elapsedSec) const override;
    int GetAttackDamage() const override;

    const std::vector<Bullet>& GetBullets() const;
    std::vector<Bullet>& GetBullets();

    void ShootIfAble(float targetX, float targetY, float elapsedSec);
    void UpdateBullets(float elapsedSec);
    float GetPreferredDistance() const { return m_PreferredDistance; }

    float m_ShootCooldown;
    float m_ShootCooldownMax;

private:
    std::vector<Bullet> m_Bullets;
    float m_AttackTimer;        
    float m_AttackSpeed;        
    float m_BulletDamage;       
    float m_PreferredDistance;  
    float m_AttackRange;        
    bool m_IsShooting;         
};