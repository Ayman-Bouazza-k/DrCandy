#include "keyboard.h"
#include "mouse.h"
#include "video.h"
#include "error.h"

#include <iostream>
#include <SDL_image.h>  // added
#include <SDL_ttf.h>    // added
#include <chrono>       // added: timing for frame pacing
#include <thread>       // added: sleep_for

#ifdef _WIN32
#include <windows.h>
// Esto puede parecer extra?, pero es necesario si el proyecto 
// es de "l?ea de comandos" en Visual Studio y vamos a usar SDL  
#undef main
#endif


struct DatosVideo g_Video;

Screen::Screen(unsigned int a, unsigned int b, unsigned int bg_r, unsigned int bg_g, unsigned int bg_b)
    : resolucioX(a), resolucioY(b), background_r(bg_r), background_g(bg_g), background_b(bg_b)
{
    //inicialització llibreria gràfica
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    ScreenStatus.exit = false;
    ScreenStatus.minimized = 0;
    ScreenStatus.currentFrameNumber = 0;
    init();
}

Screen::Screen() : Screen(600, 550, 0, 0, 0)
{
}

Screen::~Screen()
{
    SDL_DestroyRenderer(g_Video.renderer);
    SDL_DestroyWindow(g_Video.window);
    g_Video.renderer = nullptr;
    g_Video.window = nullptr;

    // Quit SDL_ttf and SDL_image before SDL_Quit
    TTF_Quit();
    IMG_Quit();

    SDL_Quit();
}

void Screen::show()
{
    // Muestra la ventana
    SDL_ShowWindow(g_Video.window);
}

void Screen::processEvents()
{
    // Borramos el buffer de vídeo antes de hacer la espera porque así aprovechamos mejor
    // el tiempo en vez de esperar para luego ponernos a borrar cuando nos toca procesar
    clearBackBuffer(background_r, background_g, background_b);

    //Procesamos teclado y mouse
    Keyboard_Tick();
    Mouse_Tick();

    // Procesamos los eventos que pueda habernos metido en la cola el sistema operativo
    processEventSystem();
}


void Screen::update()
{
    // Incrementamos el n?mero de frame actual
    ScreenStatus.currentFrameNumber++;

    // Pedimos al sistema operativo que actualice el contenido de la ventana
    flip();
}

void Screen::init()
{
    // Pedimos que se inicialice la ventana del juego y establecemos como límite mínimo MAX_FPS
    initializeHardware(resolucioX, resolucioY, MAX_FPS, RUN_IN_WINDOW_MODE);

#ifdef _WIN32
    //Alta prioridad, mejora la estabilidad del dibujado y el sonido
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
}

bool Screen::isExit() const
{
    return ScreenStatus.exit;
}

// Inicializa la ventana con el tamaño especificado
void Screen::init(int new_resx, int new_resy, bool FullScreen)
{
    Uint32 flags = SDL_WINDOW_HIDDEN; /*SDL_WINDOW_OPENGL | */
    //flags |= SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_MAXIMIZED*/ ;

    if (FullScreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    g_Video.resx = new_resx;
    g_Video.resy = new_resy;

    // Create the window where we will draw.
    g_Video.window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, new_resx, new_resy, flags);

#ifdef DONT_USE_SDL_RENDERER_SOFTWARE
    g_Video.renderer = SDL_CreateRenderer(g_Video.window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
#else
    g_Video.renderer = SDL_CreateRenderer(g_Video.window, -1, SDL_RENDERER_SOFTWARE);
    //En virtual box crashea si no es este flag
#endif

    if (!g_Video.renderer)
        throw custom_error("[Game::Video_Init] No se ha podido crear la ventana para renderizar", __FILE__, __LINE__);
    //{
    //  SDL_Rect rect ;
    //
    //  rect.h = new_resx ;
    //  rect.w = new_resy ;
    //
    //  rect.x = 0 ;
    //  rect.y = 0 ;
    //
    //  SDL_RenderSetViewport(g_Video.renderer, &rect) ;
    //}
}

// Borra el buffer de vídeo
void Screen::clearBackBuffer(int r, int g, int b)
{
    SDL_SetRenderDrawColor(g_Video.renderer, (Uint8)r, (Uint8)g, (Uint8)b, 255);
    SDL_RenderClear(g_Video.renderer);
}

void Screen::processEventSystem()
{
    SDL_Event e;
    // Mientras haya eventos en la cola nos quedamos en el bucle y los procesamos
    while (SDL_PollEvent(&e))
    {
        // Application events        
        if (e.type == SDL_QUIT) ScreenStatus.exit = true;

            // Window events        
        else if (e.type == SDL_WINDOWEVENT)
        {
            if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) ScreenStatus.minimized = true;
            else if (e.window.event == SDL_WINDOWEVENT_RESTORED) ScreenStatus.minimized = false;
        }

        // Keyboard events
        else if (e.type == SDL_KEYDOWN)
        {
            // Quitar comentario si s?o queremos hacemos caso de pulsaciones "reales" y no producto de los settings de repetici?
            //        if(!event.key.repeat) {
            Keyboard_ProcessKeyDown(e.key.keysym.sym);
            //        }
        }
        else if (e.type == SDL_KEYUP) Keyboard_ProcessKeyUp(e.key.keysym.sym);

            // Mouse events

            /**< Mouse button pressed */
        else if (e.type == SDL_MOUSEBUTTONDOWN) Mouse_ProcessButtonDown(e.button.button);
            /**< Mouse button released */
        else if (e.type == SDL_MOUSEBUTTONUP) Mouse_ProcessButtonUp(e.button.button);
            /**< Mouse wheel motion */
        else if (e.type == SDL_MOUSEWHEEL) Mouse_ProcessWheel(e.wheel.x, e.wheel.y);
    }
}


void Screen::flip()
{
    // Present the rendered frame immediately
    SDL_RenderPresent(g_Video.renderer);

    // Enforce ~60 FPS by sleeping the remainder of the 1/60s frame interval.
    // We measure time between consecutive presents.
    static auto last_present_time = std::chrono::steady_clock::now();
    constexpr auto target_frame_duration = std::chrono::duration<double>(1.0 / 60.0); // seconds

    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - last_present_time;
    if (elapsed < target_frame_duration)
    {
        auto sleep_duration = target_frame_duration - elapsed;
        // convert to std::chrono::milliseconds/ microseconds for sleep
        // use sleep_for to yield the CPU for the remaining time
        std::this_thread::sleep_for(sleep_duration);
    }
    // Update the last present timestamp after sleeping so intervals are measured
    last_present_time = std::chrono::steady_clock::now();
}

void Screen::initializeHardware(int new_resolution_x, int new_resolution_y, int fps_maximos, bool FullScreen)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw custom_error(std::string("Unable to init SDL: ") + std::string(SDL_GetError()), __FILE__, __LINE__);

    // Initialize SDL_image for PNG support
    int img_flags = IMG_INIT_PNG;
    int initted = IMG_Init(img_flags);
    if ((initted & img_flags) != img_flags)
    {
        throw custom_error(std::string("IMG_Init failed: ") + std::string(IMG_GetError()), __FILE__, __LINE__);
    }

    // Initialize SDL_ttf for font support
    if (TTF_Init() == -1)
    {
        throw custom_error(std::string("TTF_Init failed: ") + std::string(TTF_GetError()), __FILE__, __LINE__);
    }

    Keyboard_Init();
    Mouse_Init();
    init(new_resolution_x, new_resolution_y, FullScreen);
}
