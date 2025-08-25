#include "Renderer.h"
#include "Camera.h"
#include "../core/Constants.h"
#include <algorithm>
#include <iostream>
#include <cmath>

Renderer::Renderer()
    : m_renderer(nullptr)
    , m_camera(nullptr)
    , m_currentTarget(nullptr)
{
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(SDL_Window* window) {
    // Set texture filtering to nearest neighbor for pixel-perfect rendering
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // 0 = nearest neighbor, 1 = linear
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // Enable vsync to prevent tearing
    
    // WebGL-specific hints for better pixel rendering
    #ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    SDL_SetHint("SDL_HINT_RENDER_OPENGL_SHADERS", "0");
    // Force integer scaling and disable any sub-pixel rendering
    SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "0"); // letterbox mode
    #endif
    
    // Try hardware acceleration first, fall back to software if needed
    m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        // Fallback for WebAssembly - try without acceleration
        m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer) {
            // Final fallback - basic software renderer
            m_renderer = SDL_CreateRenderer(window, -1, 0);
            if (!m_renderer) {
                std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
                return false;
            }
        }
    }
    
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    
    // Disable logical size scaling to prevent WebGL scaling artifacts
    // SDL_RenderSetLogicalSize(m_renderer, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    
    return true;
}

void Renderer::cleanup() {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(m_renderer, 135, 206, 235, 255); // Sky blue
    SDL_RenderClear(m_renderer);
    
    m_commands.clear();
}

void Renderer::present() {
    sortAndRender();
    SDL_RenderPresent(m_renderer);
}

void Renderer::drawTexture(SDL_Texture* texture, const SDL_Rect& srcRect, const SDL_Rect& dstRect,
                          float rotation, const SDL_Point* center, SDL_RendererFlip flip, int layer) {
    if (!texture) return;
    
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.srcRect = srcRect;
    cmd.dstRect = dstRect;
    cmd.rotation = rotation;
    cmd.center = center ? *center : SDL_Point{dstRect.w/2, dstRect.h/2};
    cmd.flip = flip;
    cmd.layer = layer;
    cmd.isUI = false; // World space rendering
    
    m_commands.push_back(cmd);
}

void Renderer::drawTexture(SDL_Texture* texture, const Vector2& position, const Vector2& size,
                          const SDL_Rect* srcRect, int layer) {
    if (!texture) return;
    
    SDL_Rect src = {0, 0, 0, 0};
    if (srcRect) {
        src = *srcRect;
    } else {
        SDL_QueryTexture(texture, nullptr, nullptr, &src.w, &src.h);
    }
    
    SDL_Rect dst = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(size.x),
        static_cast<int>(size.y)
    };
    
    drawTexture(texture, src, dst, 0.0f, nullptr, SDL_FLIP_NONE, layer);
}

void Renderer::drawRectangle(const Rectangle& rect, SDL_Color color, bool filled, int layer) {
    SDL_Rect sdlRect = rect.toSDL();
    applyCamera(sdlRect);
    
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    
    if (filled) {
        SDL_RenderFillRect(m_renderer, &sdlRect);
    } else {
        SDL_RenderDrawRect(m_renderer, &sdlRect);
    }
}

void Renderer::drawLine(const Vector2& start, const Vector2& end, SDL_Color color, int layer) {
    Vector2 worldStart = start;
    Vector2 worldEnd = end;
    
    if (m_camera) {
        worldStart = worldToScreen(start);
        worldEnd = worldToScreen(end);
    }
    
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(m_renderer, 
                      static_cast<int>(worldStart.x), static_cast<int>(worldStart.y),
                      static_cast<int>(worldEnd.x), static_cast<int>(worldEnd.y));
}

void Renderer::drawCircle(const Vector2& center, float radius, SDL_Color color, bool filled, int layer) {
    Vector2 screenCenter = center;
    if (m_camera) {
        screenCenter = worldToScreen(center);
    }
    
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    
    int x = static_cast<int>(screenCenter.x);
    int y = static_cast<int>(screenCenter.y);
    int r = static_cast<int>(radius);
    
    for (int w = 0; w < r * 2; w++) {
        for (int h = 0; h < r * 2; h++) {
            int dx = r - w;
            int dy = r - h;
            if ((dx*dx + dy*dy) <= (r * r)) {
                if (filled || (dx*dx + dy*dy) > ((r-1) * (r-1))) {
                    SDL_RenderDrawPoint(m_renderer, x + dx, y + dy);
                }
            }
        }
    }
}

void Renderer::drawUITexture(SDL_Texture* texture, const SDL_Rect& srcRect, const SDL_Rect& dstRect,
                            float rotation, const SDL_Point* center, SDL_RendererFlip flip, int layer) {
    if (!texture) return;
    
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.srcRect = srcRect;
    cmd.dstRect = dstRect;
    cmd.rotation = rotation;
    cmd.center = center ? *center : SDL_Point{dstRect.w/2, dstRect.h/2};
    cmd.flip = flip;
    cmd.layer = layer;
    cmd.isUI = true; // UI rendering - no camera transformation
    
    m_commands.push_back(cmd);
}

void Renderer::drawUIRectangle(const Rectangle& rect, SDL_Color color, bool filled, int layer) {
    RenderCommand cmd;
    cmd.texture = nullptr; // Rectangle rendering doesn't use texture
    cmd.dstRect = {static_cast<int>(rect.x), static_cast<int>(rect.y), 
                   static_cast<int>(rect.w), static_cast<int>(rect.h)};
    cmd.layer = layer;
    cmd.isUI = true; // UI rendering - no camera transformation
    
    // Store color information (we'll handle this in sortAndRender)
    // For now, we'll render immediately since rectangles don't use the command system
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    if (filled) {
        SDL_RenderFillRect(m_renderer, &cmd.dstRect);
    } else {
        SDL_RenderDrawRect(m_renderer, &cmd.dstRect);
    }
}

Vector2 Renderer::screenToWorld(const Vector2& screenPos) const {
    if (!m_camera) return screenPos;
    
    Vector2 cameraPos = m_camera->getPosition();
    return Vector2(screenPos.x + cameraPos.x, screenPos.y + cameraPos.y);
}

Vector2 Renderer::worldToScreen(const Vector2& worldPos) const {
    if (!m_camera) return worldPos;
    
    Vector2 cameraPos = m_camera->getPosition();
    return Vector2(worldPos.x - cameraPos.x, worldPos.y - cameraPos.y);
}

void Renderer::setRenderTarget(SDL_Texture* target) {
    m_currentTarget = target;
    SDL_SetRenderTarget(m_renderer, target);
}

SDL_Texture* Renderer::createRenderTarget(int width, int height) {
    return SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
}

void Renderer::sortAndRender() {
    std::sort(m_commands.begin(), m_commands.end(), 
              [](const RenderCommand& a, const RenderCommand& b) {
                  return a.layer < b.layer;
              });
    
    for (const auto& cmd : m_commands) {
        SDL_Rect dstRect = cmd.dstRect;
        
        // Only apply camera transformation for non-UI elements
        if (!cmd.isUI) {
            applyCamera(dstRect);
        }
        
        SDL_RenderCopyEx(m_renderer, cmd.texture, &cmd.srcRect, &dstRect,
                        cmd.rotation, &cmd.center, cmd.flip);
    }
}

void Renderer::applyCamera(SDL_Rect& rect) const {
    if (m_camera) {
        Vector2 cameraPos = m_camera->getPosition();
        rect.x -= static_cast<int>(std::round(cameraPos.x));
        rect.y -= static_cast<int>(std::round(cameraPos.y));
    }
}