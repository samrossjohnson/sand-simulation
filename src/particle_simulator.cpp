#include "particle_simulator.h"

#include <algorithm>
#include <iostream>
#include <chrono>

#include "particle_registry.h"
#include "particle.h"

sasi::ParticleSimulator::ParticleSimulator(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_dataSize(width * height)
    , m_simulationStep(0ull)
    , m_particleRegistry(new ParticleRegistry())
    , m_particleData(new Particle[m_dataSize])
    , m_colorData(new uint32_t[m_dataSize])
    , m_particleOutOfBounds({ ParticleType::OutOfBounds, 0x00000000 })
    , m_particleVoid({ ParticleType::Void, 0xFF000000 })
{
    std::fill_n(m_particleData.get(), m_dataSize, Particle{});
    std::fill_n(m_colorData.get(), m_dataSize, 0xFF000000);

    m_particleData[m_width / 2] = make(ParticleType::Sand);
}

void sasi::ParticleSimulator::tick(double fixedDeltaTime)
{
    if (m_particleRegistry.get() == nullptr)
    {
        std::cout << "SASI (ERROR): missing particle registry on particle simulator.\n";
        return;
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();

    m_particleData[m_width / 2] = make(ParticleType::Sand);

    // Invoke the tick function for each particle.
    for (int i = 0; i < m_dataSize; ++i)
    {
        Particle& particle = m_particleData[i];
        if (particle.lastTickFrame >= m_simulationStep)
        {
            continue;
        }

        particle.lastTickFrame = m_simulationStep;

        TickFunction tickFunction = m_particleRegistry->getTickFunction(particle.type);
        if (tickFunction != nullptr)
        {
            tickFunction(this, particle, indexToX(i), indexToY(i));
        }
    }

    // Copy the color for each particle into the color data array.
    for (int i = 0; i < m_dataSize; ++i)
    {
        m_colorData[i] = m_particleData[i].color;
    }

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << "SASI: Fixed delta time is " << fixedDeltaTime << ". Particle simulation complete in " << ms_int.count() << "ms\n";

    ++m_simulationStep;
}

size_t sasi::ParticleSimulator::getColorDataSize() const
{
    return m_dataSize;
}

int sasi::ParticleSimulator::getColorDataWidth() const
{
    return m_width;
}

int sasi::ParticleSimulator::getColorDataHeight() const
{
    return m_height;
}

const uint32_t* sasi::ParticleSimulator::getColorData() const
{
    return m_colorData.get();
}

bool sasi::ParticleSimulator::isValidCoord(int x, int y) const
{
    return (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height);
}

bool sasi::ParticleSimulator::isValidIndex(int index) const
{
    return (index >= 0) && (index < m_dataSize);
}

int sasi::ParticleSimulator::indexToX(int index) const
{
    return index % m_width;
}

int sasi::ParticleSimulator::indexToY(int index) const
{
    return index / m_width;
}

int sasi::ParticleSimulator::coordToIndex(int x, int y) const
{
    return (y * m_width) + x;
}

void sasi::ParticleSimulator::setParticle(int x, int y, const Particle& particle)
{
    if (!isValidCoord(x, y))
    {
        std::cout << "SASI (WARNING): sasi::ParticleSimulator::setParticle was called with an out-of-bounds coord.\n";
        return;
    }

    setParticle(coordToIndex(x, y), particle);
}

void sasi::ParticleSimulator::setParticle(int index, const Particle& particle)
{
    if (!isValidIndex(index))
    {
        return;
    }

    m_particleData[index] = particle;
}

const sasi::Particle& sasi::ParticleSimulator::getParticle(int index) const
{
    if (!isValidIndex(index))
    {
        return m_particleOutOfBounds;
    }

    return m_particleData[index];
}

const sasi::Particle& sasi::ParticleSimulator::getParticle(int x, int y) const
{
    if (!isValidCoord(x, y))
    {
        return m_particleOutOfBounds;
    }

    return getParticle(coordToIndex(x, y));
}

void sasi::ParticleSimulator::move(int fromX, int fromY, int toX, int toY, const Particle& replace)
{
    if (!isValidCoord(fromX, fromY))
    {
        return;
    }

    // Move the particle to the new coord.
    setParticle(toX, toY, getParticle(fromX, fromY));

    // Replace the original particle.
    setParticle(fromX, fromY, replace);
}

sasi::Particle sasi::ParticleSimulator::make(sasi::ParticleType type) const
{
    MakeFunction makeFunction = m_particleRegistry->getMakeFunction(type);
    if (makeFunction != nullptr)
    {
        return makeFunction(this);
    }

    return m_particleVoid;
}

void sasi::ParticleSimulator::swap(int fromX, int fromY, int toX, int toY)
{
    if (!isValidCoord(fromX, fromY) || !isValidCoord(toX, toY))
    {
        return;
    }

    Particle temp = getParticle(fromX, fromY);
    setParticle(fromX, fromY, getParticle(toX, toY));
    setParticle(toX, toY, temp);
}