#include "pch.h"
#include "BossEnemy.h"
#include "utils.h"
#include <cmath>

BossEnemy::BossEnemy(Ellipsef shape, int hp, float walkingSpeed, int waveNumber)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::BOSS)
    , m_AttackTimer(0.f)
    , m_AttackSpeed(0.5f)
    , m_BulletDamage(2.f)
    , m_BurstCount(3 + (waveNumber / 10))
    , m_CurrentBurst(0)
    , m_BurstDelay(0.2f)
    , m_BurstTimer(0.f)
    , m_WaveNumber(waveNumber)
{
    m_PreferredDistance = 100.0f;
}

BossEnemy::~BossEnemy() {
    m_Bullets.clear();
}

void BossEnemy::Draw() const
{
    if (!m_IsAlive) return;

    utils::SetColor(Color4f(0.8f, 0.2f, 0.8f, 1.f));
    utils::FillEllipse(m_Shape);

    utils::SetColor(Color4f(1.f, 0.8f, 0.f, 1.f));

    float crownBaseY = m_Shape.center.y + m_Shape.radiusY * 0.8f;
    float crownTopY = m_Shape.center.y + m_Shape.radiusY * 1.5f;
    float crownWidth = m_Shape.radiusX * 1.5f;

    Vector2f p1(m_Shape.center.x - crownWidth / 2, crownBaseY);
    Vector2f p2(m_Shape.center.x - crownWidth / 3, crownTopY);
    Vector2f p3(m_Shape.center.x - crownWidth / 6, crownBaseY + (crownTopY - crownBaseY) * 0.6f);
    Vector2f p4(m_Shape.center.x, crownTopY);
    Vector2f p5(m_Shape.center.x + crownWidth / 6, crownBaseY + (crownTopY - crownBaseY) * 0.6f);
    Vector2f p6(m_Shape.center.x + crownWidth / 3, crownTopY);
    Vector2f p7(m_Shape.center.x + crownWidth / 2, crownBaseY);


    utils::DrawLine(p1, p2);
    utils::DrawLine(p2, p3);
    utils::DrawLine(p3, p4);
    utils::DrawLine(p4, p5);
    utils::DrawLine(p5, p6);
    utils::DrawLine(p6, p7);
    utils::DrawLine(p7, p1);


    DrawHealthBar();


    for (const Bullet& bullet : m_Bullets)
    {
        bullet.Draw();
    }
}

void BossEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;


    EnemyBase::Update(targetX, targetY, elapsedSec);


    for (size_t i = 0; i < m_Bullets.size(); ++i)
    {
        m_Bullets[i].Update(elapsedSec);
    }


    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());
}

bool BossEnemy::Attack(float elapsedSec, const Rectf& towerShape)
{
    if (!m_IsAlive) return false;

    float towerCenterX = towerShape.left + towerShape.width / 2.f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.f;

    float distance = GetDistanceToTarget(towerCenterX, towerCenterY);


    if (distance <= m_PreferredDistance * 1.5f)
    {

        if (m_CurrentBurst > 0)
        {
            m_BurstTimer += elapsedSec;

            if (m_BurstTimer >= m_BurstDelay)
            {

                float angleOffset = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.3f;

                float dx = towerCenterX - m_Shape.center.x;
                float dy = towerCenterY - m_Shape.center.y;
                float angle = std::atan2(dy, dx);


                angle += angleOffset;


                float targetX = m_Shape.center.x + std::cos(angle) * 500.f;
                float targetY = m_Shape.center.y + std::sin(angle) * 500.f;

                m_Bullets.emplace_back(
                    m_Shape.center.x, m_Shape.center.y,
                    targetX, targetY,
                    250.f,
                    static_cast<int>(m_BulletDamage)
                );

                m_CurrentBurst--;
                m_BurstTimer = 0.f;
                return true;
            }
        }
        else
        {

            m_AttackTimer += elapsedSec;

            if (m_AttackTimer >= 1.f / m_AttackSpeed)
            {
                m_CurrentBurst = m_BurstCount;
                m_BurstTimer = 0.f;
                m_AttackTimer = 0.f;
            }
        }
    }

    return false;
}

bool BossEnemy::CanAttack(float targetX, float targetY, float elapsedSec) const
{
    if (!m_IsAlive) return false;

    float distance = GetDistanceToTarget(targetX, targetY);
    return distance <= m_PreferredDistance * 1.5f && m_CurrentBurst > 0;
}

int BossEnemy::GetAttackDamage() const
{
    return static_cast<int>(m_BulletDamage);
}

const std::vector<Bullet>& BossEnemy::GetBullets() const
{
    return m_Bullets;
}

std::vector<Bullet>& BossEnemy::GetBullets()
{
    return m_Bullets;
}