#pragma once

#include <map>
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>

#include <iostream>

struct TexDeleter
    {
        void operator()(SDL_Texture *tex) const
        {
            SDL_DestroyTexture(tex);
        }
    };

class ResourceManager
{
public:
    static ResourceManager& getInstance()
    {
        static ResourceManager res;

        return res;
    }

    void setRenderer(std::shared_ptr<SDL_Renderer> const& renderer)
    {
        if (renderer == nullptr)
        {
            std::cerr << "Error: Renderer is not initialized." << std::endl;
            return;
        }
        this->renderer = renderer;
    }

    bool addTexture(std::string const& texturePath)
    {
        SDL_Surface *surface = IMG_Load(texturePath.c_str());
        if (surface == nullptr)
        {
            std::cerr << "Error occured while creating surface for texture: " << SDL_GetError() << std::endl;
            return false;
        }
        //SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer.get(), surface);
        std::unique_ptr<SDL_Texture, TexDeleter> tex = std::unique_ptr<SDL_Texture, TexDeleter>(SDL_CreateTextureFromSurface(renderer.get(), surface));
        SDL_FreeSurface(surface);

        if (tex == nullptr)
        {
            std::cerr << "Error occured while creating texture: " << SDL_GetError() << std::endl;
            return false;
        }
        textures[texturePath] = std::move(tex);

        return true;
    };

    SDL_Texture* getTexture(std::string const& texturePath) const
    {
        auto it = textures.find(texturePath);
        if (it == textures.end())
        {
            return nullptr;
        }
        return it->second.get(); 
    }
private:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::unique_ptr<SDL_Texture, TexDeleter>> textures{};

    std::shared_ptr<SDL_Renderer> renderer;
};