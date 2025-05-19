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
    bool Attack(float elapsedSec, const Rectf& towerShape) override;
    bool CanAttack(float targetX, float targetY, float elapsedSec) const override;
    int GetAttackDamage() const override;
    EnemyType GetType() const override { return EnemyType::Boss; }

    const std::vector<Bullet>& GetBullets() const;
    std::vector<Bullet>& GetBullets();
    int GetPowerLevel() const;
    float GetBulletDamage() const { return m_BulletDamage; }
    float GetAttackSpeed() const { return m_AttackSpeed; }
    void SetBulletDamage(float val) { m_BulletDamage = val; }
    void SetAttackSpeed(float val) { m_AttackSpeed = val; }

protected:
    Color4f GetDrawColor() const override { return Color4f(1.0f, 0.9f, 0.1f, 1.0f); }

private:
    std::vector<Bullet> m_Bullets;      

    float m_AttackTimer = 0.f;          
    float m_AttackSpeed = 0.5f;         
    float m_BulletDamage = 2.f;         

    int m_BurstCount = 3;               
    int m_CurrentBurst = 0;            
    float m_BurstDelay = 0.2f;          
    float m_BurstTimer = 0.f;          

    int m_WaveNumber = 1;               
    int m_PowerLevel = 1;               

    float m_PreferredDistance = 100.f;  

    bool m_IsAlive = true;              
    float m_WalkingSpeed = 0.f;      

    float GetDistanceToTarget(float x, float y) const;
    void DrawHealthBar() const;
};