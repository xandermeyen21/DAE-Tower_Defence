#pragma once
#include "EnemyBase.h"

class MeleeEnemy : public EnemyBase {
public:
    MeleeEnemy(Ellipsef shape, int hp, float walkingSpeed);
    bool Attack(float elapsedSec, Rectf& towerShape) override;
};
