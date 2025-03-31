#include "pch.h"
#include "Enemie.h"
#include "utils.h"

Enemie::Enemie(Ellipsef enemie, int hp, float WalkingSpeed)
	:m_WalkingSpeed{WalkingSpeed}
	,m_Hp{hp}
	,m_enemie{enemie}
{
}

void Enemie::Draw()
{
	utils::SetColor(Color4f(1.f, 0.f, 0.2f, 1.f));
	utils::FillEllipse(m_enemie);
}

void Enemie::Update()
{
}
