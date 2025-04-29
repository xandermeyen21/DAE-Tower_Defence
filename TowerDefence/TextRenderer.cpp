#include "pch.h"
#include "TextRenderer.h"
#include "utils.h"

TextRenderer::TextRenderer()
    : m_pFont(nullptr)
{
}

TextRenderer::~TextRenderer()
{
    CleanupTextures();
    if (m_pFont)
    {
        TTF_CloseFont(m_pFont);
        m_pFont = nullptr;
    }
}

bool TextRenderer::Initialize(const std::string& fontPath, int fontSize)
{
    // Clean up any existing font
    if (m_pFont)
    {
        TTF_CloseFont(m_pFont);
        m_pFont = nullptr;
    }

    // Load the font
    m_pFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!m_pFont)
    {
        std::string errorMsg = "Failed to load font: ";
        errorMsg += TTF_GetError();
        utils::LogError(errorMsg);
        return false;
    }

    return true;
}

void TextRenderer::RenderText(const std::string& text, float x, float y,
    const Color4f& color, TextAlign align)
{
    if (!m_pFont || text.empty())
        return;

    TextureInfo textureInfo = GetTextTexture(text, color);

    // Calculate position based on alignment
    float drawX = x;

    switch (align)
    {
    case TextAlign::Center:
        drawX -= textureInfo.width / 2.0f;
        break;
    case TextAlign::Right:
        drawX -= textureInfo.width;
        break;
    }

    // Create the source and destination rectangles
    SDL_Rect srcRect{ 0, 0, textureInfo.width, textureInfo.height };
    SDL_Rect dstRect{ static_cast<int>(drawX), static_cast<int>(y),
                      textureInfo.width, textureInfo.height };

    // Get the current renderer
    SDL_Renderer* pRenderer = SDL_GL_GetCurrentWindow() ? SDL_GetRenderer(SDL_GL_GetCurrentWindow()) : nullptr;
    if (pRenderer)
    {
        // Render the texture
        SDL_RenderCopy(pRenderer, textureInfo.pTexture, &srcRect, &dstRect);
    }
}

void TextRenderer::RenderTextWithBackground(const std::string& text, float x, float y,
    const Color4f& textColor, const Color4f& bgColor,
    float padding, TextAlign align)
{
    if (!m_pFont || text.empty())
        return;

    // Get text dimensions
    float textWidth, textHeight;
    GetTextDimensions(text, textWidth, textHeight);

    // Calculate background rectangle dimensions
    float bgWidth = textWidth + (padding * 2);
    float bgHeight = textHeight + (padding * 2);

    float bgX = x;
    switch (align)
    {
    case TextAlign::Center:
        bgX -= bgWidth / 2.0f;
        break;
    case TextAlign::Right:
        bgX -= bgWidth;
        break;
    }

    // Draw background
    utils::SetColor(bgColor);
    utils::FillRect(Rectf(bgX, y, bgWidth, bgHeight));

    // Draw text
    RenderText(text, x, y + padding, textColor, align);
}

void TextRenderer::GetTextDimensions(const std::string& text, float& width, float& height)
{
    width = 0;
    height = 0;

    if (!m_pFont || text.empty())
        return;

    int w, h;
    TTF_SizeText(m_pFont, text.c_str(), &w, &h);

    width = static_cast<float>(w);
    height = static_cast<float>(h);
}

TextRenderer::TextureInfo TextRenderer::GetTextTexture(const std::string& text, const Color4f& color)
{
    // Create a unique key for the text+color combo
    std::string cacheKey = text + "_" +
        std::to_string(color.r) + "_" +
        std::to_string(color.g) + "_" +
        std::to_string(color.b) + "_" +
        std::to_string(color.a);

    // Check if we already have this text cached
    auto it = m_TextCache.find(cacheKey);
    if (it != m_TextCache.end())
    {
        return it->second;
    }

    // Convert Color4f to SDL_Color
    SDL_Color sdlColor{
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    };

    // Create a surface from the text
    SDL_Surface* pSurface = TTF_RenderText_Blended(m_pFont, text.c_str(), sdlColor);
    if (!pSurface)
    {
        utils::LogError("Failed to render text surface");
        return { nullptr, 0, 0 };
    }

    // Get the current renderer
    SDL_Renderer* pRenderer = SDL_GL_GetCurrentWindow() ? SDL_GetRenderer(SDL_GL_GetCurrentWindow()) : nullptr;
    if (!pRenderer)
    {
        SDL_FreeSurface(pSurface);
        utils::LogError("Failed to get renderer");
        return { nullptr, 0, 0 };
    }

    // Create texture from surface
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // Store width and height
    int width = pSurface->w;
    int height = pSurface->h;

    // Free the surface
    SDL_FreeSurface(pSurface);

    if (!pTexture)
    {
        utils::LogError("Failed to create texture from rendered text");
        return { nullptr, 0, 0 };
    }

    // Store in cache
    TextureInfo info = { pTexture, width, height };
    m_TextCache[cacheKey] = info;

    return info;
}

void TextRenderer::CleanupTextures()
{
    for (auto& pair : m_TextCache)
    {
        if (pair.second.pTexture)
        {
            SDL_DestroyTexture(pair.second.pTexture);
            pair.second.pTexture = nullptr;
        }
    }
    m_TextCache.clear();
}
