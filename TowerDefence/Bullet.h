#pragma once
#include "structs.h" 

class Bullet
{
public:
    Bullet(float x, float y, float targetX, float targetY, float speed, int damage, int hp = 1);

    Bullet& operator=(const Bullet& other)
    {
        if (this != &other)
        {
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;
            m_Speed = other.m_Speed;
            m_Damage = other.m_Damage;
            m_IsActive = other.m_IsActive;
            m_HP = other.m_HP;
        }
        return *this;
    }
    void Deactivate() { m_IsActive = false; }
    void Draw() const;
    void Update(float elapsedSec, float windowWidth, float windowHeight);
    bool IsActive() const;
    bool CheckHit(const Ellipsef& enemyShape) const;
    int GetDamage() const;
    Vector2f GetPosition() const { return m_Position; }
    float GetSpeed() const { return m_Speed; }


    int GetHP() const { return m_HP; }
    void TakeDamage() { m_HP--; if (m_HP <= 0) m_IsActive = false; }
    void SetTarget(float targetX, float targetY);

private:
    Vector2f m_Position;
    Vector2f m_Direction;
    float m_Speed;
    int m_Damage;
    bool m_IsActive;
    int m_HP;  
    const float m_Radius{ 5.f };
};