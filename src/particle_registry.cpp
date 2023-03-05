#include "particle_registry.h"

#include "particle/sand.h"
#include "particle/water.h"

sasi::ParticleRegistry::ParticleRegistry()
    : m_tickFunctions({})
    , m_makeFunctions({})
{
    m_tickFunctions.insert({ ParticleType::Sand, sandTick });
    m_makeFunctions.insert({ ParticleType::Sand, sandMake });
    m_tickFunctions.insert({ ParticleType::Water, waterTick });
    m_makeFunctions.insert({ ParticleType::Water, waterMake });
}

sasi::TickFunction sasi::ParticleRegistry::getTickFunction(ParticleType type) const
{
    if (auto it = m_tickFunctions.find(type); it != m_tickFunctions.end())
    {
        return it->second;
    }

    return nullptr;
}

sasi::MakeFunction sasi::ParticleRegistry::getMakeFunction(ParticleType type) const
{
    if (auto it = m_makeFunctions.find(type); it != m_makeFunctions.end())
    {
        return it->second;
    }

    return nullptr;
}
