#pragma once

#include <cstdint>
#include <limits>

namespace sasi
{
    enum class ParticleType : uint32_t
    {
        Void = 0,
        OutOfBounds,

        Sand,
        Water,

        Max = std::numeric_limits<uint32_t>::max()
    };

    struct Particle
    {
        Particle()
            : type(ParticleType::Void)
            , color(0x000000)
            , lastTickFrame(0ull)
        {
        }

        Particle(ParticleType _type, uint32_t _color)
            : type(_type)
            , color(_color)
            , lastTickFrame(0ull)
        {
        }

        ParticleType type;
        uint32_t color;
        uint64_t lastTickFrame;
    };
}