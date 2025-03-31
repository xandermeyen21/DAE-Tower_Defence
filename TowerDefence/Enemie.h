#pragma once
class Enemie
{
public:
	Enemie(Ellipsef enemie, int hp, float WalkingSpeed);
	void Draw();
	void Update();
private:
	float m_WalkingSpeed;
	int m_Hp;
	Ellipsef m_enemie;
};

