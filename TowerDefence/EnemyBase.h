#pragma once
#include "structs.h"
#include <vector>

enum class EnemyType {
    Melee,
    Ranged,
    Boss
};

class Tower;

class EnemyBase
{
public:
    EnemyBase(const Ellipsef& shape, int hp, float walkingSpeed, EnemyType type);
    virtual ~EnemyBase() = default;

    virtual void Update(float elapsedSec);
    virtual void Update(float elapsedSec, const std::vector<Tower*>& towers) { Update(elapsedSec); }
    virtual void Update(float targetX, float targetY, float elapsedSec, float windowWidth, float windowHeight);
    virtual void Update(float targetX, float targetY, float elapsedSec);
    virtual void Draw() const;


    const Ellipsef& GetShape() const;
    float GetHealth() const;
    float GetMaxHealth() const;
    float GetDamage() const;
    float GetSpeed() const;
    bool IsAlive() const;

    virtual bool Attack(float elapsedSec, const Rectf& towerShape) = 0;
    virtual bool CanAttack(float targetX, float targetY, float elapsedSec) const { return false; }
    virtual int GetAttackDamage() const { return 0; }

    void TakeDamage(float damage);
    void SetPosition(const Vector2f& position);

    virtual EnemyType GetType() const = 0;

    void SetTarget(const Vector2f& target);
    bool HasReachedTarget() const;

protected:
    virtual Color4f GetDrawColor() const { return Color4f(0.7f, 0.2f, 0.2f, 1.0f); }
    Ellipsef m_Shape;
    float m_Health;
    float m_MaxHealth;
    float m_Damage;
    float m_Speed;
    Vector2f m_Target;
    bool m_HasReachedTarget;
    EnemyType m_Type;
};