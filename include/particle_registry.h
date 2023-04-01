#pragma once

#include <unordered_map>

#include "particle.h"

namespace sasi
{
    class ParticleSimulator;

    typedef Coord (*TickFunction)(ParticleSimulator* simulator, Particle& particle, const Coord& coord, float deltaTime);
    typedef Particle (*MakeFunction)(const ParticleSimulator* simulator);

    class ParticleRegistry
    {
    public:
        ParticleRegistry();

        TickFunction getTickFunction(ParticleType type) const;
        MakeFunction getMakeFunction(ParticleType type) const;

    private:
        std::unordered_map<ParticleType, TickFunction> m_tickFunctions;
        std::unordered_map<ParticleType, MakeFunction> m_makeFunctions;
    };
}