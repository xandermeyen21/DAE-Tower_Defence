#pragma once
#include "structs.h" 
#include <vector>


class EnemyBase;
class Bullet;

class Tower
{
public:
    Tower(Rectf tower, float range, float damage = 1.f);
    void Draw() const;
    void Update(float elapsedSec, const std::vector<EnemyBase*>& enemies);
    const Rectf& GetPosition() const;
    void UpgradeDamage(float amount);
    void UpgradeRange(float amount);
    void UpgradeAttackSpeed(float amount);
    float GetDamage() const;
    float GetRange() const;
    float GetAttackSpeed() const;
    std::vector<Bullet>& GetBullets();
    Rectf GetRect() const { return m_Tower; }

private:
    float m_Range;
    float m_Damage;
    float m_AttackSpeed;
    float m_AttackTimer;
    Rectf m_Tower;
    std::vector<Bullet> m_Bullets;
};