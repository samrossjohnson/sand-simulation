#include "engine/engine.h"

#include <iostream>

#include "world.h"

sasi::Engine::Engine(int initWidth, int initHeight)
    : m_frame(0ULL)
    , m_isInitialized(false)
    , m_isQuitting(false)
    , m_clearView(0)
    , m_inputState({})
{
    // Initialize SDL video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SASI (ERROR): Failed to initialize SDL. SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    m_window.reset(SDL_CreateWindow(
        "Sand Simulation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        initWidth,
        initHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if (m_window.get() == nullptr)
    {
        std::cout << "SASI (ERROR): Failed to create SDL window. SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    // Cache starting width and height.
    SDL_GetWindowSize(m_window.get(), &m_windowWidth, &m_windowHeight);

    // Provide bgfx with window information.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    if (!SDL_GetWindowWMInfo(m_window.get(), &windowInfo))
    {
        std::cout << "SASI (ERROR): Failed to access SDL window info. SDL_Error: " << SDL_GetError() << "\n";
        return;
    }
    bgfx::Init init;
    init.platformData.ndt = windowInfo.info.x11.display;
    init.platformData.nwh = (void*)(uintptr_t)windowInfo.info.x11.window;
    init.resolution.width = (uint32_t)m_windowWidth;
    init.resolution.height = (uint32_t)m_windowHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
    {
        std::cout << "SASI (ERROR): Failed to initialize BGFX.\n";
        return;
    }

    bgfx::setViewClear(
        m_clearView,
        BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL | BGFX_CLEAR_COLOR,
        0x000000FF);
    bgfx::setViewRect(m_clearView, 0, 0, uint16_t(m_windowWidth), uint16_t(m_windowHeight));

    // Init succeeded.
    m_isInitialized = true;

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    m_world.reset(new World{ 160, 160, SDL_GetTicks64(), &m_inputState });
}

sasi::Engine::~Engine()
{
    // Temp reset world before shutting down bgfx to release bgfx resources until
    // they are wrapped in a renderer.
    m_world.reset();
    bgfx::shutdown();
}

bool sasi::Engine::tick()
{
    while (!m_isQuitting)
    {
        m_inputState.refreshKeyStates();

        pollEvents();

        bgfx::touch(m_clearView);

        if (m_world.get() != nullptr)
        {
            const Uint64 elapsedTimeMs = SDL_GetTicks64();

            m_world->tick(m_frame, elapsedTimeMs);

            m_world->updateBackbufferWidthHeight(m_windowWidth, m_windowHeight);
            m_world->render(m_frame, m_clearView);
        }

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
        ++m_frame;

        return false;
    }

    return true;
}

bool sasi::Engine::isInitialized() const
{
    return m_isInitialized;
}

void sasi::Engine::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_QUIT)
        {
            m_isQuitting = true;
        }
        else if (event.type == SDL_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                int newWidth;
                int newHeight;
                SDL_GetWindowSize(m_window.get(), &newWidth, &newHeight);
                if ((m_windowWidth != newWidth) || (m_windowHeight != newHeight))
                {
                    m_windowWidth = newWidth;
                    m_windowHeight = newHeight;

                    bgfx::reset((uint32_t)m_windowWidth, (uint32_t)m_windowHeight, BGFX_RESET_VSYNC);
                    bgfx::setViewRect(m_clearView, 0, 0, bgfx::BackbufferRatio::Equal);
                }
                break;
            default:
                break;
            }
        }
        else if (event.type == SDL_KEYDOWN)
        {
            m_inputState.setKeyDown(event.key.keysym.sym);
        }
        else if (event.type == SDL_KEYUP)
        {
            m_inputState.setKeyUp(event.key.keysym.sym);
        }
    }
}