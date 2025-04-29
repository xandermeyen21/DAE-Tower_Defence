#pragma once
#include "pch.h"
#include <string>
#include <map>
#include <memory>

class TextRenderer
{
public:
    enum class TextAlign
    {
        Left,
        Center,
        Right
    };

    TextRenderer();
    ~TextRenderer();

    
    bool Initialize(const std::string& fontPath, int fontSize);

    
    void RenderText(const std::string& text, float x, float y,
        const Color4f& color = Color4f(1.0f, 1.0f, 1.0f, 1.0f),
        TextAlign align = TextAlign::Left);

  
    void RenderTextWithBackground(const std::string& text, float x, float y,
        const Color4f& textColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f),
        const Color4f& bgColor = Color4f(0.0f, 0.0f, 0.0f, 0.7f),
        float padding = 5.0f,
        TextAlign align = TextAlign::Left);

    
    void GetTextDimensions(const std::string& text, float& width, float& height);

private:
    TTF_Font* m_pFont;
    struct TextureInfo
    {
        SDL_Texture* pTexture;
        int width;
        int height;
    };

   
    std::map<std::string, TextureInfo> m_TextCache;

   
    TextureInfo GetTextTexture(const std::string& text, const Color4f& color);

  
    void CleanupTextures();
};

