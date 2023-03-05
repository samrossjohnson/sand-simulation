#include "world.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <fstream>

#include <bgfx/bgfx.h>

#include "shader_util.h"

bgfx::VertexLayout sasi::PosUvVertex::ms_layout;

static const int k_targetFps = 30;
static const double k_targetFrameTime = 1.0 / k_targetFps;

sasi::World::World(int width, int height, uint64_t startTimeMs)
    : m_simulator({width, height})
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
    double elapsedTimeSecs = elapsedTimeMs / 1000.0;
    double deltaTimeSecs = elapsedTimeSecs - m_previousTickTimeSecs;

    m_previousTickTimeSecs = elapsedTimeSecs;
    m_fixedTimeAccumulationSecs += deltaTimeSecs;

    while (m_fixedTimeAccumulationSecs >= k_targetFrameTime)
    {
        m_simulator.tick(k_targetFrameTime);

        m_fixedTimeAccumulationSecs -= k_targetFrameTime;
    }
}

void sasi::World::render(int frame, bgfx::ViewId viewId)
{
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