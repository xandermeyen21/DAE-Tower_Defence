#pragma once
#include "EnemyBase.h"
#include "Bullet.h"
#include <vector>

class Tower;

class RangedEnemy : public EnemyBase
{
public:
    RangedEnemy(const Vector2f& position, float health, float damage, float speed);
    RangedEnemy(const Ellipsef& shape, float health, float speed);

    virtual ~RangedEnemy() = default;

    void Update(float elapsedSec, const std::vector<Tower*>& towers) override;
    void Update(float targetX, float targetY, float elapsedSec);
    void Draw() const override;
    bool Attack(float elapsedSec, const Rectf& towerShape) override;

    EnemyType GetType() const override { return EnemyType::Ranged; }
    int GetAttackDamage() const override;
    bool CanAttack(float targetX, float targetY, float elapsedSec) const override;

    void ShootIfAble(float targetX, float targetY, float elapsedSec);
    void ShootBullet(float targetX, float targetY); 
    void UpdateBullets(float elapsedSec);

    std::vector<Bullet>& GetBullets();
    const std::vector<Bullet>& GetBullets() const;

    bool IsShooting() const;
    float GetAttackRange() const;

    float m_BulletDamage;
    float m_ShootCooldownMax;
    float m_ShootCooldown;

private:
    bool m_IsShooting;
    float m_AttackTimer;
    float m_AttackSpeed;
    float m_AttackRange;
    std::vector<Bullet> m_Bullets;
};