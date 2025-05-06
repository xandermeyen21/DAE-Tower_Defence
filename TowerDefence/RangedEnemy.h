#pragma once
#include "EnemyBase.h"
#include "Bullet.h"
#include <vector>

class RangedEnemy : public EnemyBase {
public:
    RangedEnemy(Ellipsef shape, int hp, float walkingSpeed);

    void Draw() const override;
    void Update(float targetX, float targetY, float elapsedSec) override;
    bool Attack(float elapsedSec, Rectf& towerShape) override;

    const std::vector<Bullet>& GetBullets() const;
    std::vector<Bullet>& GetBullets();

private:
    std::vector<Bullet> m_Bullets;
    float m_AttackTimer;
    float m_AttackSpeed; 
    float m_BulletDamage;
};
