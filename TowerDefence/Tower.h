#pragma once
#include "structs.h" 
#include <vector>
#include <algorithm>

class EnemyBase;
class Bullet;

class Tower
{
public:
    Tower(Rectf tower, float range, float damage = 1.f);
    void Draw() const;
    void Update(float elapsedSec, const std::vector<EnemyBase*>& enemies, float windowWidth, float windowHeight);
    const Rectf& GetPosition() const;

    void ResetToInitialStats();
    void ClearBullets();
    void UpgradeDamage(float amount);
    void UpgradeRange(float amount);
    void UpgradeAttackSpeed(float amount);
    void UpgradeMaxHealth(float amt);
    void UpgradeRicochet(int amount);

    void IncreaseDamage(float amount);
    void IncreaseRange(float amount);
    void IncreaseAttackSpeed(float amount);
    void IncreaseRicochet(int amount); 

    float GetDamage() const;
    float GetRange() const;
    float GetAttackSpeed() const;
    std::vector<Bullet>& GetBullets();
    Rectf GetRect() const { return m_Tower; }
    int GetMaxHealth() const { return m_MaxHealth; }
    int GetHealth() const { return m_Health; }
    int GetRicochetCount() const;

    void Heal(int amount) {
        m_Health = std::min(m_Health + amount, m_MaxHealth);
    }

    void SetPosition(const Rectf& rect);

private:
    float m_Range;
    float m_Damage;
    float m_AttackSpeed;
    float m_AttackTimer;
    Rectf m_Tower;
    std::vector<Bullet> m_Bullets;
    int m_MaxHealth{ 100 };
    int m_Health{ 100 };
    int m_RicochetCount = 0;
};