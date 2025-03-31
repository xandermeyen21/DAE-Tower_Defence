#pragma once
#include "Bullet.h"

class Tower
{
public:
    Tower(Rectf tower, float range, float damage = 5.f);
    void Draw() const; // Made Draw() const
    void Update();
    const Rectf& GetPosition() const;

private:
    float m_Range;
    float m_Damage;
    //float m_AttackSpeed;
    Rectf m_Tower;
};