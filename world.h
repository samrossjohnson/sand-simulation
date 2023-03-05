#pragma once

#include <bgfx/bgfx.h>

#include "particle_simulator.h"

namespace sasi
{
    struct PosUvVertex
    {
        float x;
        float y;
        float z;
        float u;
        float v;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    static const PosUvVertex s_triVertices[] =
    {
        { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
        { 0.5f, -0.5f, 0.0f, 1.0f, 1.0f },
        { -0.5f, 0.5f, 0.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f, 0.0f, 1.0f, 0.0f }
    };

    static const uint16_t s_triIndices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    class World
    {
    private:
        ParticleSimulator m_simulator;
        bgfx::VertexBufferHandle m_vbh;
        bgfx::IndexBufferHandle m_ibh;
        bgfx::ProgramHandle m_ph;
        bgfx::TextureHandle m_textureHandle;
        bgfx::UniformHandle m_textureSampler;

        uint64_t m_startTimeMs;
        double m_previousTickTimeSecs;
        double m_fixedTimeAccumulationSecs;

    public:
        World(int width, int height, uint64_t startTimeMs);
        ~World();

        void tick(int frame, uint64_t elapsedTimeMs);
        void render(int frame, bgfx::ViewId viewId);
    };
}