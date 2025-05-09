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
    if (isSelected)
    {
        utils::SetColor(Color4f(1.0f, 0.0f, 1.0f, 0.6f));
    }
    else
    {
        switch (m_Type)
        {
        case UpgradeType::DAMAGE:
            utils::SetColor(Color4f(0.8f, 0.2f, 0.2f, 0.4f));
            break;
        case UpgradeType::ATTACK_SPEED:
            utils::SetColor(Color4f(0.2f, 0.8f, 0.2f, 0.4f));
            break;
        case UpgradeType::RANGE:
            utils::SetColor(Color4f(0.2f, 0.2f, 0.8f, 0.4f));
            break;
        case UpgradeType::REPAIR:
            utils::SetColor(Color4f(0.8f, 0.8f, 0.2f, 0.4f));
            break;
        default:
            utils::SetColor(Color4f(0.4f, 0.4f, 0.4f, 0.4f));
            break;
        }
    }
    utils::FillRect(Rectf(x, y, width, height));

    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 0.8f));
    utils::DrawRect(Rectf(x, y, width, height), 2.0f);

    Texture nameText(m_Name, "ShortBaby.ttf", 15, Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
    nameText.Draw(Vector2f(x + 10.f, y + height - 15.f));

    Texture descText(m_Description, "ShortBaby.ttf", 14, Color4f{ 1.0f, 1.0f, 1.0f, 0.8f });
    descText.Draw(Vector2f(x + 10.f, y + height - 35.f));

    // Draw type icon (optional enhancement)
    // To be implemented in a future version
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
        "Repair Tower",
        "Restore " + std::to_string(static_cast<int>(amount)) + " tower health",
        amount,
        nullptr  
    );
}