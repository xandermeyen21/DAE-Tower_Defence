#include "pch.h"
#include "RangedEnemy.h"
#include <cmath>
#include "utils.h"

RangedEnemy::RangedEnemy(Ellipsef shape, int hp, float walkingSpeed)
    : EnemyBase(shape, hp, walkingSpeed, EnemyType::RANGED)
    , m_AttackTimer(0.0f)
    , m_AttackSpeed(1.0f)
    , m_BulletDamage(1.0f)
    , m_PreferredDistance(100.0f)
    , m_AttackRange(150.0f)
    , m_IsShooting(false)
    , m_ShootCooldown(0.0f)
    , m_ShootCooldownMax(1.0f)
{
}

void RangedEnemy::Update(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distSq = dx * dx + dy * dy;
    float distance = std::sqrt(distSq);

    m_IsShooting = (distance <= m_AttackRange);

    if (m_IsShooting) {
        m_ShootCooldown -= elapsedSec;

        if (std::abs(distance - m_PreferredDistance) > 5.0f) {
            if (distance > 0.1f) {
                dx /= distance;
                dy /= distance;

                if (distance > m_PreferredDistance) {
                    m_Shape.center.x += dx * m_WalkingSpeed * elapsedSec;
                    m_Shape.center.y += dy * m_WalkingSpeed * elapsedSec;
                }
                else {
                    m_Shape.center.x -= dx * m_WalkingSpeed * 0.5f * elapsedSec;
                    m_Shape.center.y -= dy * m_WalkingSpeed * 0.5f * elapsedSec;
                }
            }
        }
    }
    else {
        if (distance > 0.1f) {
            dx /= distance;
            dy /= distance;
            m_Shape.center.x += dx * m_WalkingSpeed * elapsedSec;
            m_Shape.center.y += dy * m_WalkingSpeed * elapsedSec;
        }
    }

    for (auto it = m_Bullets.begin(); it != m_Bullets.end();) {
        it->Update(elapsedSec);

        if (!it->IsActive()) {
            it = m_Bullets.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool RangedEnemy::Attack(float elapsedSec, const Rectf& towerShape)
{
    if (!m_IsAlive) return false;

    float towerCenterX = towerShape.left + towerShape.width / 2.f;
    float towerCenterY = towerShape.bottom + towerShape.height / 2.f;

    float dx = towerCenterX - m_Shape.center.x;
    float dy = towerCenterY - m_Shape.center.y;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length > 0) {
        dx /= length;
        dy /= length;
    }

    float bulletSpeed = 200.0f;
    m_Bullets.emplace_back(
        m_Shape.center.x, m_Shape.center.y,
        towerCenterX, towerCenterY,
        bulletSpeed,
        static_cast<int>(m_BulletDamage)
    );

    return true;
}

bool RangedEnemy::CanAttack(float targetX, float targetY, float elapsedSec) const
{
    if (!m_IsAlive) return false;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distSq = dx * dx + dy * dy;

    return distSq <= m_AttackRange * m_AttackRange;
}

int RangedEnemy::GetAttackDamage() const
{
    return static_cast<int>(m_BulletDamage);
}

void RangedEnemy::Draw() const
{
    EnemyBase::Draw();

    utils::SetColor(Color4f(0.8f, 0.2f, 1.0f, 1.0f));
    utils::FillEllipse(m_Shape.center, m_Shape.radiusX, m_Shape.radiusY);

    float cx = m_Shape.center.x;
    float cy = m_Shape.center.y;
    float r = std::max(m_Shape.radiusX, m_Shape.radiusY);

    std::vector<Vector2f> triangle = {
        Vector2f(cx, cy + r),
        Vector2f(cx - r * 0.6f, cy - r * 0.3f),
        Vector2f(cx + r * 0.6f, cy - r * 0.3f)
    };
    utils::SetColor(Color4f(1.0f, 0.4f, 1.0f, 1.0f));
    utils::FillPolygon(triangle);

    for (const Bullet& bullet : m_Bullets) {
        bullet.Draw();
    }
}

const std::vector<Bullet>& RangedEnemy::GetBullets() const
{
    return m_Bullets;
}

std::vector<Bullet>& RangedEnemy::GetBullets()
{
    return m_Bullets;
}

void RangedEnemy::ShootIfAble(float targetX, float targetY, float elapsedSec)
{
    if (!m_IsAlive) return;

    float dx = targetX - m_Shape.center.x;
    float dy = targetY - m_Shape.center.y;
    float distSq = dx * dx + dy * dy;
    float distance = std::sqrt(distSq);

    if (distance <= m_AttackRange && m_ShootCooldown <= 0) {
        float bulletSpeed = 75.0f;

        m_Bullets.emplace_back(
            m_Shape.center.x, m_Shape.center.y,
            targetX, targetY,
            bulletSpeed,
            static_cast<int>(m_BulletDamage)
        );

        m_ShootCooldown = m_ShootCooldownMax;
    }
}


void RangedEnemy::UpdateBullets(float elapsedSec)
{
    for (Bullet& bullet : m_Bullets) {
        bullet.Update(elapsedSec);
    }

    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const Bullet& b) { return !b.IsActive(); }),
        m_Bullets.end());
}