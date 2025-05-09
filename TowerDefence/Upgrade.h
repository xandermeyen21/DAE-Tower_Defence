#pragma once
#include <string>
#include <functional>
#include "Tower.h"

enum class UpgradeType
{
    DAMAGE,
    ATTACK_SPEED,
    RANGE,
    REPAIR,
    
};

class Upgrade
{
public:
    Upgrade(UpgradeType type, const std::string& name, const std::string& description,
        float amount, std::function<void(Tower&, float)> applyEffect);
    ~Upgrade() = default;

    UpgradeType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    const std::string& GetDescription() const { return m_Description; }
    float GetAmount() const { return m_Amount; }

    void Apply(Tower& tower) const;

    void Draw(float x, float y, float width, float height, bool isSelected) const;

    static Upgrade CreateDamageUpgrade(float amount);
    static Upgrade CreateAttackSpeedUpgrade(float amount);
    static Upgrade CreateRangeUpgrade(float amount);
    static Upgrade CreateRepairUpgrade(float amount);

private:
    UpgradeType m_Type;
    std::string m_Name;
    std::string m_Description;
    float m_Amount;
    std::function<void(Tower&, float)> m_ApplyEffect;
};