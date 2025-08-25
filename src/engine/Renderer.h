#pragma once

#include "../core/Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <vector>

struct RenderCommand {
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    float rotation;
    SDL_Point center;
    SDL_RendererFlip flip;
    int layer;
    bool isUI; // Flag to indicate this should not be affected by camera transformation
    
    RenderCommand() : texture(nullptr), srcRect{0,0,0,0}, dstRect{0,0,0,0}, 
                     rotation(0.0f), center{0,0}, flip(SDL_FLIP_NONE), 
                     layer(0), isUI(false) {}
};

class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(SDL_Window* window);
    void cleanup();
    
    void clear();
    void present();
    
    void drawTexture(SDL_Texture* texture, const SDL_Rect& srcRect, const SDL_Rect& dstRect, 
                    float rotation = 0.0f, const SDL_Point* center = nullptr, 
                    SDL_RendererFlip flip = SDL_FLIP_NONE, int layer = 0);
    
    void drawTexture(SDL_Texture* texture, const Vector2& position, const Vector2& size,
                    const SDL_Rect* srcRect = nullptr, int layer = 0);
    
    void drawRectangle(const Rectangle& rect, SDL_Color color, bool filled = true, int layer = 0);
    void drawLine(const Vector2& start, const Vector2& end, SDL_Color color, int layer = 0);
    void drawCircle(const Vector2& center, float radius, SDL_Color color, bool filled = true, int layer = 0);
    
    // UI-specific rendering methods (not affected by camera transformation)
    void drawUITexture(SDL_Texture* texture, const SDL_Rect& srcRect, const SDL_Rect& dstRect, 
                      float rotation = 0.0f, const SDL_Point* center = nullptr, 
                      SDL_RendererFlip flip = SDL_FLIP_NONE, int layer = 0);
    void drawUIRectangle(const Rectangle& rect, SDL_Color color, bool filled = true, int layer = 0);
    
    void setCamera(Camera* camera) { m_camera = camera; }
    Camera* getCamera() const { return m_camera; }
    
    SDL_Renderer* getSDLRenderer() const { return m_renderer; }
    
    Vector2 screenToWorld(const Vector2& screenPos) const;
    Vector2 worldToScreen(const Vector2& worldPos) const;
    
    void setRenderTarget(SDL_Texture* target);
    SDL_Texture* createRenderTarget(int width, int height);

private:
    void sortAndRender();
    void applyCamera(SDL_Rect& rect) const;
    
    SDL_Renderer* m_renderer;
    Camera* m_camera;
    
    std::vector<RenderCommand> m_commands;
    SDL_Texture* m_currentTarget;
};