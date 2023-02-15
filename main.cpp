#include <iostream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

SDL_Window* g_window = nullptr;
const int g_initWidth = 640;
const int g_initHeight = 480;
const uint32_t g_clearColor = 0x000000ff;

int main(int argc, char* argv[])
{
    // Initialize SDL video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize SDL. SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create the window.
    g_window = SDL_CreateWindow(
        "Sand Simulation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        g_initWidth,
        g_initHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (g_window == nullptr)
    {
        std::cout << "Failed to create SDL window. SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Cache starting width and height.
    int width;
    int height;
    SDL_GetWindowSize(g_window, &width, &height);

    // Provide bgfx with window information.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    if (!SDL_GetWindowWMInfo(g_window, &windowInfo))
    {
        std::cout << "Failed to access SDL window info. SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }
    bgfx::Init init;
    init.platformData.ndt = windowInfo.info.x11.display;
    init.platformData.nwh = (void*)(uintptr_t)windowInfo.info.x11.window;
    init.resolution.width = (uint32_t)g_initWidth;
    init.resolution.height = (uint32_t)g_initHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;

    /** Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create
        a render thread. Most graphics APIs must be used on the same thread that
        created the window. */
    bgfx::renderFrame();

    if (!bgfx::init(init))
    {
        std::cout << "Failed to initialize BGFX.\n";
        return -1;
    }

    const bgfx::ViewId clearView = 0;
    bgfx::setViewClear(
        clearView,
        BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL | BGFX_CLEAR_COLOR,
        g_clearColor);
    bgfx::setViewRect(clearView, 0, 0, uint16_t(g_initWidth), uint16_t(g_initHeight));

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Run event loop until user quit.
    bool shouldQuit = false;
    SDL_Event event;
    while (!shouldQuit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                shouldQuit = true;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        // Handle window size changes.
                        int newWidth;
                        int newHeight;
                        SDL_GetWindowSize(g_window, &newWidth, &newHeight);
                        if ((width != newWidth) || (height != newHeight))
                        {
                            width = newWidth;
                            height = newHeight;

                            bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
                            bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
                        }

                        break;

                    // Not all events are managed currently so we expect to get here.
                    default:
                        break;
                }
            }

        }

        bgfx::touch(clearView);

        // Write resolution stats to debug text.
        bgfx::dbgTextClear();
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(
            0,
            0,
            0x0f,
            "Backbuffer %dW x %dH in pixels.",
            stats->width,
            stats->height);

        bgfx::frame();
    }

    bgfx::shutdown();
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
