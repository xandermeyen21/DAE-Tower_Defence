#pragma once
#include "EnemyBase.h"
#include "Bullet.h"
#include <vector>

class BossEnemy : public EnemyBase {
public:
    BossEnemy(Ellipsef shape, int hp, float walkingSpeed, int waveNumber);
    virtual ~BossEnemy();
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
    int m_BurstCount; 
    int m_CurrentBurst;
    float m_BurstDelay;
    float m_BurstTimer;
    int m_WaveNumber; 
};