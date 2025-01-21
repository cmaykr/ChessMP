#pragma once

#include <map>
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

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
            exit(1);
            return;
        }
        this->renderer = renderer;
    }

    bool loadTexture(std::string const& texturePath)
    {
        std::unique_ptr<SDL_Texture, TexDeleter> tex = std::unique_ptr<SDL_Texture, TexDeleter>(IMG_LoadTexture(renderer.get(), texturePath.c_str()));

        if (tex == nullptr)
        {
            std::cerr << "Error occured while creating texture: " << SDL_GetError() << std::endl;
            exit(1);
            return false;
        }
        if (textures[texturePath] != nullptr)
        {
            std::cerr << "Error: Texture already loaded, not loading again." << std::endl;
            SDL_DestroyTexture(tex.get());
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

    bool loadTexturesFromFile(std::string const& filename)
    {
        std::ifstream jsonFile{filename};

        nlohmann::json data = nlohmann::json::parse(jsonFile);

        std::string basePath = data["basePath"];
        basePath += "/";
        for (std::string texture : data["textures"])
        {
            std::cout << "Loading texture: " << basePath + texture << std::endl;
            loadTexture(basePath + texture);
        }

        return true;
    }
private:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::unique_ptr<SDL_Texture, TexDeleter>> textures{};

    std::shared_ptr<SDL_Renderer> renderer;
};