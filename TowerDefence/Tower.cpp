#include "pch.h"
#include "Tower.h"
#include "utils.h"

Tower::Tower(Rectf tower, float range, float damage)
    :m_Range{ range }
    , m_Damage{ damage }
    , m_Tower{ tower }
{
}

void Tower::Draw() const // Made Draw() const
{
    utils::SetColor(Color4f(0.f, 1.f, 1.f, 1.f));
    utils::FillRect(m_Tower);
}

void Tower::Update()
{
}

const Rectf& Tower::GetPosition() const
{
    return m_Tower;
}