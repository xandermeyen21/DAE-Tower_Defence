#pragma once
#include "structs.h" 

class Bullet
{
public:
    Bullet(float x, float y, float targetX, float targetY, float speed, int damage, int ricochetLeft = 0);
   
    Bullet& operator=(const Bullet& other)
    {
        if (this != &other)
        {
            m_Position = other.m_Position;
            m_Direction = other.m_Direction;
            m_Speed = other.m_Speed;
            m_Damage = other.m_Damage;
            m_IsActive = other.m_IsActive;
            // m_Radius is const, so we don't copy it
        }
        return *this;
    }
    void Deactivate() { m_IsActive = false; }
    void Draw() const;
    void Update(float elapsedSec);
    bool IsActive() const;
    bool CheckHit(const Ellipsef& enemyShape) const;
    int GetDamage() const;
    Vector2f GetPosition() const { return m_Position; }
    int m_RicochetLeft = 0;
    float GetSpeed() const { return m_Speed; }

private:
    Vector2f m_Position;
    Vector2f m_Direction;
    float m_Speed;
    int m_Damage;
    bool m_IsActive;
    const float m_Radius{ 5.f };
};