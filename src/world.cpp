#include "world.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <fstream>

#include <bgfx/bgfx.h>

#include "sasi.h"

#include "camera.h"
#include "shader_util.h"
#include "engine/input_state.h"

bgfx::VertexLayout sasi::PosUvVertex::ms_layout;

static const int k_targetFps = 30;
static const double k_targetFrameTime = 1.0 / k_targetFps;

sasi::World::World(int width, int height, uint64_t startTimeMs, const InputState* inputState)
    : m_simulator({ 160, 160 })
    , m_camera(new Camera{ 0.0f, 100.0f })
    , m_inputState(inputState)
    , m_startTimeMs(startTimeMs)
    , m_previousTickTimeSecs(0.0)
    , m_fixedTimeAccumulationSecs(0.0)
{
    PosUvVertex::init();
    m_vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(s_triVertices, sizeof(s_triVertices)),
        PosUvVertex::ms_layout
    );
    m_ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(s_triIndices, sizeof(s_triIndices))
    );

    bgfx::ShaderHandle vsh{};
    if (!loadShader(vsh, "bin/shaders/v_simple.bin"))
    {
        std::cout << "SASI: World initialization failure. Failed to load vertex shader.\n";
    }
    bgfx::ShaderHandle fsh{};
    if (!loadShader(fsh, "bin/shaders/f_simple.bin"))
    {
        std::cout << "SASI: World initialization failure. Failed to load fragment shader.\n";
    }
    m_ph = bgfx::createProgram(vsh, fsh, true);

    // Create the texture we'll use to render the world.
    m_textureHandle = bgfx::createTexture2D(
        width,
        height,
        false,
        0,
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE,
        nullptr
    );

    // Create sampler to use with texture.
    m_textureSampler = bgfx::createUniform(
        "s_texColor",
        bgfx::UniformType::Sampler);
}

sasi::World::~World()
{
    bgfx::destroy(m_textureSampler);
    bgfx::destroy(m_textureHandle);
    bgfx::destroy(m_ph);
    bgfx::destroy(m_ibh);
    bgfx::destroy(m_vbh);
}

void sasi::World::tick(int frame, uint64_t elapsedTimeMs)
{
    if (m_inputState == nullptr)
    {
        std::cout << "SASI (ERROR): world is missing input state.\n";
        return;
    }

    if (m_camera.get() == nullptr)
    {
        std::cout << "SASI (ERROR): world is missing camera.\n";
        return;
    }

    double elapsedTimeSecs = elapsedTimeMs / 1000.0;
    double deltaTimeSecs = elapsedTimeSecs - m_previousTickTimeSecs;

    m_previousTickTimeSecs = elapsedTimeSecs;
    m_fixedTimeAccumulationSecs += deltaTimeSecs;

    const float cameraSpeed = 5.0f;
    if (m_inputState->isKeyDownThisFrame(SDLK_i))
    {
        m_camera->zoomIn();
    }
    if (m_inputState->isKeyDownThisFrame(SDLK_o))
    {
        m_camera->zoomOut();
    }
    if (m_inputState->isKeyDown(SDLK_d))
    {
        m_camera->translate(bx::mul(bx::Vec3{ 1.0f, 0.0f, 0.0f }, cameraSpeed * deltaTimeSecs ));
    }
    if (m_inputState->isKeyDown(SDLK_a))
    {
        m_camera->translate(bx::mul(bx::Vec3{ -1.0f, 0.0f, 0.0f }, cameraSpeed * deltaTimeSecs ));
    }
    if (m_inputState->isKeyDown(SDLK_w))
    {
        m_camera->translate(bx::mul(bx::Vec3{ 0.0f, 1.0f, 0.0f }, cameraSpeed * deltaTimeSecs ));
    }
    if (m_inputState->isKeyDown(SDLK_s))
    {
        m_camera->translate(bx::mul(bx::Vec3{ 0.0f, -1.0f, 0.0f }, cameraSpeed * deltaTimeSecs ));
    }

    if (m_inputState->isMouseButtonDown(1))
    {
        int x, y;
        m_inputState->getMouseLocation(x, y);
        const bx::Vec3 mouseWorldLocation = m_camera->screenToWorldLocation(m_backbufferWidth, m_backbufferHeight, x, y);
        m_simulator.makeParticleAtWorldLocation(ParticleType::Sand, mouseWorldLocation);
    }

    // Run fixed update simulation.
    while (m_fixedTimeAccumulationSecs >= k_targetFrameTime)
    {
        m_simulator.tick(k_targetFrameTime);

        m_fixedTimeAccumulationSecs -= k_targetFrameTime;
    }
}

void sasi::World::render(int frame, bgfx::ViewId viewId)
{
    if (m_camera.get() == nullptr)
    {
        std::cout << "SASI (ERROR): World:: failed to render as camera is null.\n";
        return;
    }

    float view[16];
    m_camera->getView(view);
    float proj[16];
    m_camera->getproj(proj, m_backbufferWidth, m_backbufferHeight);
    bgfx::setViewTransform(viewId, view, proj);

    // Set render states.
    bgfx::setState(BGFX_STATE_DEFAULT);

    // Set model matrix.
    float mtxScale[16];
    bx::mtxScale(mtxScale, 20.0f, 20.0f, 1.0f);
    float mtxTranslate[16];
    bx::mtxTranslate(mtxTranslate, 0.0f, -1.0f, 0.0f);
    float mtxTransform[16];
    bx::mtxMul(mtxTransform, mtxTranslate, mtxScale);
    bgfx::setTransform(mtxTransform);

    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);

    bgfx::updateTexture2D(
        m_textureHandle,
        0, // Mip level
        0, // X offset
        0, // Y offset
        0, // Z offset (for 3D textures)
        m_simulator.getColorDataWidth(), // Width of the new data
        m_simulator.getColorDataHeight(), // Height of the new data
        bgfx::makeRef(static_cast<const void*>(m_simulator.getColorData()), m_simulator.getColorDataSize()));

    // Bind texture.
    bgfx::setTexture(0, m_textureSampler, m_textureHandle, BGFX_SAMPLER_POINT);

    // Submit primitive for rendering.
    bgfx::submit(viewId, m_ph);
}

void sasi::World::updateBackbufferWidthHeight(int width, int height)
{
    m_backbufferWidth = width;
    m_backbufferHeight = height;
}