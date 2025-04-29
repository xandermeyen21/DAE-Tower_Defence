#pragma once
class Enemie
{
public:
    Enemie(Ellipsef enemie, int hp, float WalkingSpeed);
    void Draw() const;
    void Update(float targetX, float targetY, float elapsedSec);
    bool TakeDamage(int damage);
    bool IsAlive() const;
    const Ellipsef& GetShape() const;
    float GetDistanceToTarget(float targetX, float targetY) const;
    bool HasReachedTarget(float targetX, float targetY) const;

private:
    float m_WalkingSpeed;
    int m_Hp;
    Ellipsef m_enemie;
    bool m_IsAlive;
};
