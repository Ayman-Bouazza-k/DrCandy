/**
 *  Simplified, platform-independent access to the SDL wrapper by Enric Vergara in March 2021.
 *  
 *  Students need not change this file, but they should get familiar 
 *  with the public interface of GraphicManager.
 */

#pragma once

// These definitions allow to include SDL in different platforms
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined  (_WIN64)
#include <iostream>
// Necessary definition so SDL can be included and find main
#define SDL_MAIN_HANDLED
#include <windows.h>
// Graphics library
#include "libreria.h"
#include "NFont/NFont.h"
#include <conio.h>      /* getch */ 
#elif __APPLE__
// Graphics library
#include "private/sdl_wrapper/libreria.h"
#include "private/sdl_wrapper/NFont/NFont.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <SDL2/SDL.h>
#pragma clang diagnostic pop
#else
// Linux
#include <iostream>
#define SDL_MAIN_HANDLED
#include "private/sdl_wrapper/libreria.h"
#include "private/sdl_wrapper/NFont/NFont.h"
#endif
#include "controller.h"
#include <stdio.h>
#include <string>
#include <map>
#include <filesystem>
#include <vector>
using namespace std;

/// Run a graphic game loop for the given game object until ESC is pressed.
template <typename GameClass>
void runGraphicGame(GameClass& game,
                    int screenWidth = 980, int screenHeight = 980,
                    int bgRed = 255, int bgGreen = 255, int bgBlue = 255);

/// Main class to manipulate graphics by drawing images and text.
/// Don't try to create instances of this class directly: the object is passed
/// to Game::render automatically. 
class GraphicManager
{
public:
    /// Draw an image resource at the given position.
    /// @param resource_name the name of the image resource to draw, 
    ///   relative to the `1. Resources/data/` folder, e.g., "img/figure.png".
    void drawImage(const std::string& resource_name, int x, int y);

    /// Draw text at the given position with the given scale and color.
    /// Color values must be in the range 0-255.
    void drawText(const std::string& text, int x, int y,
                  int font_size = 32, int red = 0, int green = 0, int blue = 0);

    /// Draw a rectangle outline at (x,y) with width w and height h.
    /// Thickness is in pixels. Color values must be in the range 0-255.
    void drawRectangle(int x, int y, int w, int h,
                       int thickness = 3, int red = 0, int green = 0, int blue = 0);

    /// Get all image resource names available in the data dir. This names are the ones
    /// expected by draw_image.
    std::vector<std::string> getImageNames();

private:
    /// Singleton instance
    static GraphicManager* m_instance;
    /// Map of preloaded sprites
    std::map<std::string, Sprite*> m_sprites;

    GraphicManager();
    virtual ~GraphicManager();
    GraphicManager(GraphicManager const&);
    static GraphicManager* get_instance();
    GraphicManager& operator=(GraphicManager const&);

    template <typename GameClass>
    friend void runGraphicGame(GameClass& game, int screenWidth, int screenHeight,
                               int bgRed, int bgGreen, int bgBlue);
};

// Implementation of the main graphic loop. No need to change anything here.
template <typename GameClass>
void runGraphicGame(GameClass& game, int screenWidth, int screenHeight,
                    int bgRed, int bgGreen, int bgBlue)
{
    // Ensure SDL is ready and main can be set by the library
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    Screen screen(screenWidth, screenHeight, bgRed, bgGreen, bgBlue);
    GraphicManager& graphicManager = *GraphicManager::get_instance();
    Controller controller;

    screen.show();

    do
    {
        // Capture all mouse and keyboard events from the last cycle
        screen.processEvents();
        controller.update();

        game.update(controller);
        game.render(graphicManager);

        screen.update();
    }
    while (!Keyboard_GetKeyTrg(KEYBOARD_ESCAPE));

    SDL_Quit();
}
