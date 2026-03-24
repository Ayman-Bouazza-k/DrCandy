#include "graphics.h"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <SDL_ttf.h>
#include <cmath>
#include "util.h"

// Single definition of the static instance
GraphicManager* GraphicManager::m_instance = nullptr;

// Constructor implementation
GraphicManager::GraphicManager()
{
}

// Delete all preloaded sprites
GraphicManager::~GraphicManager()
{
    for (auto& pair : m_sprites)
    {
        delete pair.second;
    }
}

GraphicManager* GraphicManager::get_instance()
{
    if (m_instance == NULL)
        m_instance = new GraphicManager();
    return m_instance;
}

void GraphicManager::drawImage(const std::string& resource_name, int x, int y)
{
    auto found = m_sprites.find(resource_name);

    if (found == m_sprites.end())
    {
        // Lazy-load the sprite now that we're very likely to have an initialized renderer.
        std::string fullpath = getDataDirPath() + resource_name;
        bool image_exists = true;
        if (!std::filesystem::exists(fullpath))
        {
            std::cout << "Error: image path '" << fullpath << "' does not exist.\n";
            image_exists = false;
        }

        if (image_exists)
        {
            Sprite* s = nullptr;
            try
            {
                s = new Sprite();
                s->create(fullpath.c_str());
            }
            catch (const std::exception& e)
            {
                std::cout << "Failed to load sprite '" << fullpath << "': " << e.what() << "\n";
                delete s;
            }

            if (s != nullptr)
            {
                m_sprites[resource_name] = s;
                found = m_sprites.find(resource_name);
            }
        }
    }

    if (found != m_sprites.end() && found->second)
    {
        found->second->draw(static_cast<int>(x), static_cast<int>(y));
    }
}

void GraphicManager::drawText(
    const std::string& text, int x, int y, int font_size, int red, int green, int blue)
{
    std::string font_path = getDataDirPath() + "fonts/freesans.ttf";
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), font_size);
    if (!font)
    {
        std::cerr << "[draw_text] TTF_OpenFont failed for '" << font_path << "': " << TTF_GetError() << "\n";
    }
    else
    {
        SDL_Color color = {static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue), 255};
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surf)
        {
            std::cerr << "[draw_text] TTF_RenderUTF8_Blended failed: " << TTF_GetError() << "\n";
            TTF_CloseFont(font);
        }
        else
        {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_Video.renderer, surf);
            if (!tex)
            {
                std::cerr << "[draw_text] SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
                SDL_FreeSurface(surf);
                TTF_CloseFont(font);
            }
            else
            {
                SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

                SDL_Rect dst;
                dst.x = static_cast<int>(x);
                dst.y = static_cast<int>(y);
                dst.w = surf->w;
                dst.h = surf->h;

                SDL_FreeSurface(surf);

                if (SDL_RenderCopy(g_Video.renderer, tex, NULL, &dst) != 0)
                {
                    std::cerr << "[draw_text] SDL_RenderCopy failed: " << SDL_GetError() << "\n";
                }

                SDL_DestroyTexture(tex);
                TTF_CloseFont(font);
            }
        }
    }
}


void GraphicManager::drawRectangle(int x, int y, int w, int h,
                                   int thickness, int red, int green, int blue)
{
    x -= thickness;
    y -= thickness;
    w += 2 * thickness;
    h += 2 * thickness;

    // Ignore degenerate rects or non-positive thickness
    bool invalid_args = (w == 0 || h == 0 || thickness <= 0);

    if (!invalid_args)
    {
        // Support negative width/height by adjusting origin and using positive sizes
        if (w < 0)
        {
            x += w;
            w = -w;
        }
        if (h < 0)
        {
            y += h;
            h = -h;
        }

        // Clamp thickness to half the smallest dimension
        int max_th = (std::min)(w, h) / 2;
        if (thickness > max_th) thickness = max_th;
        if (thickness <= 0) return;

        // Save previous draw color
        Uint8 prev_r = 0, prev_g = 0, prev_b = 0, prev_a = 255;
        SDL_GetRenderDrawColor(g_Video.renderer, &prev_r, &prev_g, &prev_b, &prev_a);

        // Set requested color
        SDL_SetRenderDrawColor(g_Video.renderer, static_cast<Uint8>(red), static_cast<Uint8>(green),
                               static_cast<Uint8>(blue), 255);

        // Draw nested rects to simulate thickness
        for (int t = 0; t < thickness; ++t)
        {
            SDL_Rect rect;
            rect.x = x + t;
            rect.y = y + t;
            rect.w = w - 2 * t;
            rect.h = h - 2 * t;
            if (rect.w <= 0 || rect.h <= 0) break;
            SDL_RenderDrawRect(g_Video.renderer, &rect);
        }

        // Restore previous draw color
        SDL_SetRenderDrawColor(g_Video.renderer, prev_r, prev_g, prev_b, prev_a);
    }
}

std::vector<std::string> GraphicManager::getImageNames()
{
    std::vector<std::string> image_paths;
    std::filesystem::path data_dir = getDataDirPath();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(data_dir))
    {
        if (entry.is_regular_file())
        {
            std::string extension = entry.path().extension().string();
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg"
                || extension == ".PNG" || extension == ".JPG" || extension == ".JPEG")
            {
                image_paths.push_back(std::filesystem::relative(entry.path(), data_dir).generic_string());
            }
        }
    }

    return image_paths;
}
