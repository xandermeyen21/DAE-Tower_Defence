#include "pch.h"
#include "Upgrade.h"
#include "Tower.h"
#include "utils.h"
#include "Texture.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iomanip>

Upgrade::Upgrade(UpgradeType type, const std::string& name, const std::string& description,
    float amount, std::function<void(Tower&, float)> applyEffect)
    : m_Type(type)
    , m_Name(name)
    , m_Description(description)
    , m_Amount(amount)
    , m_ApplyEffect(applyEffect)
    , m_pCardTexture(nullptr)
    , m_pNameTexture(nullptr)
    , m_pDescriptionTexture(nullptr)
    , m_NameFontPath("")
    , m_DescriptionFontPath("")
    , m_NameFontSize(18)
    , m_DescriptionFontSize(14)
    , m_NameColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f))
    , m_DescriptionColor(Color4f(1.0f, 1.0f, 1.0f, 0.8f))
{
}

Upgrade::~Upgrade()
{
    CleanupTextures();
}

void Upgrade::CleanupTextures()
{
    if (m_pNameTexture) {
        delete m_pNameTexture;
        m_pNameTexture = nullptr;
    }

    if (m_pDescriptionTexture) {
        delete m_pDescriptionTexture;
        m_pDescriptionTexture = nullptr;
    }
}

void Upgrade::Apply(Tower& tower) const
{
    if (m_ApplyEffect)
    {
        m_ApplyEffect(tower, m_Amount);
    }
}

void Upgrade::SetFontStyle(const std::string& nameFontPath, const std::string& descriptionFontPath,
    int nameFontSize, int descriptionFontSize,
    const Color4f& nameColor, const Color4f& descriptionColor)
{
    m_NameFontPath = nameFontPath;
    m_DescriptionFontPath = descriptionFontPath;
    m_NameFontSize = nameFontSize;
    m_DescriptionFontSize = descriptionFontSize;
    m_NameColor = nameColor;
    m_DescriptionColor = descriptionColor;

    CreateTextTextures();
}

void Upgrade::CreateTextTextures()
{
    if (!m_NameFontPath.empty() && !m_DescriptionFontPath.empty())
    {
        CleanupTextures();

        try
        {
            m_pNameTexture = new Texture(m_Name, m_NameFontPath, m_NameFontSize, m_NameColor);
            m_pDescriptionTexture = new Texture(m_Description, m_DescriptionFontPath, m_DescriptionFontSize, m_DescriptionColor);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to create upgrade text textures: " << e.what() << std::endl;
        }
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
    else {
        utils::SetColor(Color4f(0.2f, 0.3f, 0.5f, 0.8f));
        utils::FillRect(Rectf(x, y, width, height));
        utils::SetColor(Color4f(0.4f, 0.5f, 0.8f, 0.5f));
        utils::FillRect(Rectf(x, y + height - 20.f, width, 20.f));
    }


    if (m_pNameTexture) {
        float nameX = x + (width - m_pNameTexture->GetWidth()) / 2.0f;
        float nameY = y + height - 30.f;
        m_pNameTexture->Draw(Vector2f(nameX, nameY));
    }

  
    Color4f typeColor;
    switch (m_Type) {
    case UpgradeType::DAMAGE:
        typeColor = Color4f(1.0f, 0.3f, 0.3f, 1.0f); // Red
        break;
    case UpgradeType::ATTACK_SPEED:
        typeColor = Color4f(1.0f, 0.7f, 0.3f, 1.0f); // Orange
        break;
    case UpgradeType::RANGE:
        typeColor = Color4f(0.3f, 0.7f, 1.0f, 1.0f); // Blue
        break;
    case UpgradeType::REPAIR:
        typeColor = Color4f(0.3f, 1.0f, 0.3f, 1.0f); // Green
        break;
    case UpgradeType::RICOCHET:
        typeColor = Color4f(0.8f, 0.3f, 1.0f, 1.0f); // Purple
        break;
    default:
        typeColor = Color4f(0.5f, 0.5f, 0.5f, 1.0f); // Gray
        break;
    }

   
    utils::SetColor(typeColor);
    utils::FillRect(Rectf(x + 10, y + height - 20.f, 10.f, 10.f));

    std::string amountText = "+" + std::to_string(static_cast<int>(m_Amount));
    if (m_Type == UpgradeType::ATTACK_SPEED) {
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << m_Amount;
        amountText = "+" + oss.str();
    }

    float badgeSize = 40.f;
    float badgeX = x + width - badgeSize - 10.f;
    float badgeY = y + 10.f;
    utils::SetColor(typeColor);
    utils::FillEllipse(badgeX + badgeSize / 2, badgeY + badgeSize / 2, badgeSize / 2, badgeSize / 2);
    utils::SetColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    utils::DrawString(Vector2f(badgeX + badgeSize / 2 - 10.f, badgeY + badgeSize / 2 + 5.f), amountText);
}

Upgrade* Upgrade::CreateDamageUpgrade(float amount)
{
    return new Upgrade(
        UpgradeType::DAMAGE,
        "Damage Up",
        "Increase tower damage by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeDamage(amt); }
    );
}

Upgrade* Upgrade::CreateAttackSpeedUpgrade(float amount)
{
    return new Upgrade(
        UpgradeType::ATTACK_SPEED,
        "Attack Speed Up",
        "Increase attack speed by " + std::to_string(amount),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeAttackSpeed(amt); }
    );
}

Upgrade* Upgrade::CreateRangeUpgrade(float amount)
{
    return new Upgrade(
        UpgradeType::RANGE,
        "Range Up",
        "Increase tower range by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeRange(amt); }
    );
}

Upgrade* Upgrade::CreateRepairUpgrade(float amount)
{
    return new Upgrade(
        UpgradeType::REPAIR,
        "Max Health Up",
        "Increase max tower health by " + std::to_string(static_cast<int>(amount)),
        amount,
        [](Tower& tower, float amt) { tower.UpgradeMaxHealth(amt); }
    );
}

Upgrade* Upgrade::CreateRicochetUpgrade(float amount)
{
    return new Upgrade(
        UpgradeType::RICOCHET,
        "Ricochet",
        "Bullets bounce to " + std::to_string(static_cast<int>(amount)) + " more enemies",
        amount,
        [](Tower& tower, float amt) {
            tower.UpgradeRicochet(static_cast<int>(amt));
        }
    );
}