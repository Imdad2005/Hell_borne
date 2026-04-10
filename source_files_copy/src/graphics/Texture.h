#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class Texture {
private:
    SDL_Texture* texture;
    int width;
    int height;
    SDL_Color fallbackColor;
    bool loaded;

public:
    Texture() : texture(nullptr), width(0), height(0), loaded(false) {
        fallbackColor = {0, 200, 255, 255}; // Cyan default
    }

    ~Texture() {
        free();
    }

    bool loadFromFile(SDL_Renderer* renderer, const std::string& path) {
        free();
        
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface == nullptr) {
            SDL_Log("Unable to load image %s! IMG_Error: %s", path.c_str(), IMG_GetError());
            loaded = false;
            return false;
        }

        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture == nullptr) {
            SDL_Log("Unable to create texture from %s! SDL Error: %s", path.c_str(), SDL_GetError());
            loaded = false;
        } else {
            width = loadedSurface->w;
            height = loadedSurface->h;
            loaded = true;
        }

        SDL_FreeSurface(loadedSurface);
        return loaded;
    }

    void free() {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
            width = 0;
            height = 0;
            loaded = false;
        }
    }

    void setColor(Uint8 red, Uint8 green, Uint8 blue) {
        if (texture != nullptr) {
            SDL_SetTextureColorMod(texture, red, green, blue);
        }
    }

    void setBlendMode(SDL_BlendMode blending) {
        if (texture != nullptr) {
            SDL_SetTextureBlendMode(texture, blending);
        }
    }

    void setAlpha(Uint8 alpha) {
        if (texture != nullptr) {
            SDL_SetTextureAlphaMod(texture, alpha);
        }
    }

    void setFallbackColor(SDL_Color color) {
        fallbackColor = color;
    }

    void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr, 
                double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        SDL_Rect renderQuad = {x, y, width, height};

        if (clip != nullptr) {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        if (loaded && texture != nullptr) {
            SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
        } else {
            // Fallback: render colored rectangle
            SDL_SetRenderDrawColor(renderer, fallbackColor.r, fallbackColor.g, 
                                   fallbackColor.b, fallbackColor.a);
            SDL_RenderFillRect(renderer, &renderQuad);
        }
    }

    void renderFallback(SDL_Renderer* renderer, SDL_Rect dst) {
        SDL_SetRenderDrawColor(renderer, fallbackColor.r, fallbackColor.g, 
                               fallbackColor.b, fallbackColor.a);
        SDL_RenderFillRect(renderer, &dst);
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isLoaded() const { return loaded; }
};

#endif // TEXTURE_H
