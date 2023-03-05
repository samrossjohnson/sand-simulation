#include "particle_simulator.h"

#include <algorithm>
#include <iostream>
#include <chrono>

// ================ SAND ================
sasi::Particle sandMake(const sasi::ParticleSimulator* simulator)
{
    return sasi::Particle{ sasi::ParticleType::Sand, 0xFF00FFFF };
}

void sandTick(sasi::ParticleSimulator* simulator, sasi::Particle& particle, int x, int y)
{
    // If the cell below is free, move into it.
    const sasi::Particle& below = simulator->getParticle(x, y + 1);
    if (below.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the below type is a liquid we can move through, swap us.
    if (below.type == sasi::ParticleType::Water)
    {
       simulator->swap(x, y, x, y + 1);
    }

    // If the cell below-left is free, move into it.
    const sasi::Particle& belowLeft = simulator->getParticle(x - 1, y + 1);
    if (belowLeft.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x - 1, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-right is free, move into it.
    const sasi::Particle& belowRight = simulator->getParticle(x + 1, y + 1);
    if (belowRight.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x + 1, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }
}
// ================ END OF SAND ================

// ================ WATER ================
sasi::Particle waterMake(const sasi::ParticleSimulator* simulator)
{
    return sasi::Particle{ sasi::ParticleType::Water, 0xFFFF0000 };
}

void waterTick(sasi::ParticleSimulator* simulator, sasi::Particle& particle, int x, int y)
{
    // If the cell below is free, move into it.
    const sasi::Particle& below = simulator->getParticle(x, y + 1);
    if (below.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-left is free, move into it.
    const sasi::Particle& belowLeft = simulator->getParticle(x - 1, y + 1);
    if (belowLeft.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x - 1, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-right is free, move into it.
    const sasi::Particle& belowRight = simulator->getParticle(x + 1, y + 1);
    if (belowRight.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x + 1, y + 1, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell left is free spread.
    const sasi::Particle& left = simulator->getParticle(x - 1, y);
    if (left.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x - 1, y, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell right is free spread.
    const sasi::Particle& right = simulator->getParticle(x + 1, y);
    if (right.type == sasi::ParticleType::Void)
    {
        simulator->move(x, y, x + 1, y, simulator->make(sasi::ParticleType::Void));
        return;
    }
}
// ================ END OF WATER ================

sasi::ParticleSimulator::ParticleSimulator(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_dataSize(width * height)
    , m_particleData(new Particle[m_dataSize])
    , m_colorData(new uint32_t[m_dataSize])
    , m_tickFunctions({})
    , m_particleOutOfBounds({ ParticleType::OutOfBounds, 0x00000000 })
    , m_particleVoid({ ParticleType::Void, 0xFF000000 })
{
    m_tickFunctions.insert({ ParticleType::Sand, sandTick });
    m_makeFunctions.insert({ ParticleType::Sand, sandMake });
    m_tickFunctions.insert({ ParticleType::Water, waterTick });
    m_makeFunctions.insert({ ParticleType::Water, waterMake });

    std::fill_n(m_particleData.get(), m_dataSize, Particle{});
    std::fill_n(m_colorData.get(), m_dataSize, 0xFF000000);
}

void sasi::ParticleSimulator::tick(int frame)
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();

    // Invoke the tick function for each particle.
    for (int i = 0; i < m_dataSize; ++i)
    {
        Particle& particle = m_particleData[i];
        if (particle.lastTickFrame >= frame)
        {
            continue;
        }

        particle.lastTickFrame = frame;

        if (auto it = m_tickFunctions.find(particle.type); it != m_tickFunctions.end())
        {
            it->second(this, particle, indexToX(i), indexToY(i));
        }
    }

    // Copy the color for each particle into the color data array.
    for (int i = 0; i < m_dataSize; ++i)
    {
        m_colorData[i] = m_particleData[i].color;
    }

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << "SASI: Particle simulation complete in " << ms_int.count() << "ms\n";
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
    if (auto it = m_makeFunctions.find(type); it != m_makeFunctions.end())
    {
        return it->second(this);
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