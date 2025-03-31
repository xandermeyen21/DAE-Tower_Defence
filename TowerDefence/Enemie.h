#pragma once
class Enemie
{
public:
    Enemie(Ellipsef enemie, int hp, float WalkingSpeed);
    void Draw() const; 
    void Update(float targetX, float targetY, float elapsedSec);

private:
    float m_WalkingSpeed;
    int m_Hp;
    Ellipsef m_enemie;
};

