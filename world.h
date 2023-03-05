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

    public:
        World(int width, int height);
        ~World();

        void tick(int frame);
        void render(int frame, bgfx::ViewId viewId);

        //int getWidth() const;
        //int getHeight() const;
        //const Particle* getData() const;
        //size_t getDataSize() const;

    private:
        //int indexToX(int index) const;
        //int indexToY(int index) const;
        //int xyToIndex(int x, int y) const;

        //void setParticle(int index, Particle particle);
        //void setParticle(int x, int y, Particle particle);
        //Particle getParticle(int index) const;
        //Particle getParticle(int x, int y) const;
    };
}