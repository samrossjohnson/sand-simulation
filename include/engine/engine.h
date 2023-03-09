#pragma once

#include <cstdint>
#include <memory>

#include <bgfx/bgfx.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

namespace sasi
{
    class World;

    struct SDLWindowDeleter
    {
        inline void operator()(SDL_Window* window)
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    };
    
    class Engine
    {
    public:
        Engine(int initWidth, int initHeight);
        ~Engine();

        bool tick();

        bool isInitialized() const;
    
    private:
        void pollEvents();

    private:
        uint64_t m_frame;
        bool m_isInitialized;
        bool m_isQuitting;

        const bgfx::ViewId m_clearView;

        int m_windowWidth;
        int m_windowHeight;

        std::unique_ptr<SDL_Window, SDLWindowDeleter> m_window;
        std::unique_ptr<World> m_world;
    };
}