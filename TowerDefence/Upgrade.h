#pragma once

#include <string>
#include <functional>
#include "Tower.h"
#include "Texture.h"

enum class UpgradeType
{
    DAMAGE,
    ATTACK_SPEED,
    RANGE,
    REPAIR,
    RICOCHET
};

class Upgrade
{
public:
    Upgrade(UpgradeType type, const std::string& name, const std::string& description,
        float amount, std::function<void(Tower&, float)> applyEffect);
    ~Upgrade();

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
    static Upgrade CreateRicochetUpgrade(float amount);

    void SetTexture(Texture* texture) { m_pCardTexture = texture; }
    Texture* GetTexture() const { return m_pCardTexture; }

   
    void SetFontStyle(const std::string& nameFontPath, const std::string& descriptionFontPath,
        int nameFontSize, int descriptionFontSize,
        const Color4f& nameColor, const Color4f& descriptionColor);
    void CreateTextTextures();  
    void CleanupTextures();    

private:
    UpgradeType m_Type;
    std::string m_Name;
    std::string m_Description;
    float m_Amount;
    std::function<void(Tower&, float)> m_ApplyEffect;
    Texture* m_pCardTexture = nullptr;

    Texture* m_pNameTexture = nullptr;        
    Texture* m_pDescriptionTexture = nullptr; 
    std::string m_NameFontPath;              
    std::string m_DescriptionFontPath;       
    int m_NameFontSize;                      
    int m_DescriptionFontSize;               
    Color4f m_NameColor;                     
    Color4f m_DescriptionColor;              
};