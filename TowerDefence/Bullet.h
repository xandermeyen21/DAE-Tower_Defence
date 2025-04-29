#pragma once

class Bullet
{
public:
    Bullet(float x, float y, float targetX, float targetY, float speed, int damage);
    void Draw() const;
    void Update(float elapsedSec);
    bool IsActive() const;
    bool CheckHit(const Ellipsef& enemyShape) const;
    int GetDamage() const;

private:
    Vector2f m_Position;
    Vector2f m_Direction;
    float m_Speed;
    int m_Damage;
    bool m_IsActive;
    const float m_Radius{ 5.f }; // Bullet size
};

