#pragma once
#include "structs.h"

enum class EnemyType {
    MELEE,
    RANGED,
    BOSS
};

class EnemyBase {
public:
    EnemyBase(Ellipsef shape, int hp, float walkingSpeed, EnemyType type);
    virtual ~EnemyBase() = default;

    virtual void Draw() const;
    virtual void Update(float targetX, float targetY, float elapsedSec);
    virtual bool Attack(float elapsedSec, Rectf& towerShape);
    bool TakeDamage(int damage);
    bool IsAlive() const;
    const Ellipsef& GetShape() const;
    float GetDistanceToTarget(float targetX, float targetY) const;
    bool HasReachedTarget(float targetX, float targetY, float minDistance) const;
    EnemyType GetType() const;
    int GetHp() const;
    int GetMaxHp() const;
    
protected:
    void DrawHealthBar() const; 
    float m_WalkingSpeed;
    int m_Hp;
    int m_MaxHp;
    Ellipsef m_Shape;
    bool m_IsAlive;
    EnemyType m_Type;
    float m_PreferredDistance;
};