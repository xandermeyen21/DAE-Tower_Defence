#include "pch.h"
#include "Upgrade.h"
#include "Tower.h"
#include "utils.h"
#include "Texture.h"

Upgrade::Upgrade(UpgradeType type, const std::string& name, const std::string& description,
    float amount, std::function<void(Tower&, float)> applyEffect)
    : m_Type(type)
    , m_Name(name)
    , m_Description(description)
    , m_Amount(amount)
    , m_ApplyEffect(applyEffect)
{
}

void Upgrade::Apply(Tower& tower) const
{
    if (m_ApplyEffect)
    {
        m_ApplyEffect(tower, m_Amount);
    }
}

void Upgrade::Draw(float x, float y, float width, float height, bool isSelected) const
{
    
    if (isSelected) {
        utils::SetColor(Color4f(1.0f, 0.9f, 0.3f, 0.6f));
        utils::FillRect(Rectf(x - 6, y - 6, width + 12, height + 12));
    }

    
    if (m_pCardTexture) {
        m_pCardTexture->Draw(Rectf(x, y, width, height));
    }
}


Upgrade Upgrade::CreateDamageUpgrade(float amount)
{
    return Upgrade(
        UpgradeType::DAMAGE,
        "Damage Up",
        "Increase tower damage by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeDamage(amt); }
    );
}

Upgrade Upgrade::CreateAttackSpeedUpgrade(float amount)
{
    return Upgrade(
        UpgradeType::ATTACK_SPEED,
        "Attack Speed Up",
        "Increase attack speed by " + std::to_string(amount),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeAttackSpeed(amt); }
    );
}

Upgrade Upgrade::CreateRangeUpgrade(float amount)
{
    return Upgrade(
        UpgradeType::RANGE,
        "Range Up",
        "Increase tower range by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeRange(amt); }
    );
}

Upgrade Upgrade::CreateRepairUpgrade(float amount)
{
    return Upgrade(
        UpgradeType::REPAIR,
        "Max Health Up",
        "Increase max tower health by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeMaxHealth(amt); }
    );
}

Upgrade Upgrade::CreateRicochetUpgrade(float amount)
{
    return Upgrade(
        UpgradeType::RICOCHET,
        "Ricochet",
        "Bullets ricochet to " + std::to_string(static_cast<int>(amount)) + " more enemy",
        amount,
        [](Tower& tower, float amt) { tower.UpgradeRicochet(static_cast<int>(amt)); }
    );
}