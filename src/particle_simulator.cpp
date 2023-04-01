#include "particle_simulator.h"

#include "sasi.h"

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
    , m_stabilityPassesThisStep(0)
    , m_particleRegistry(new ParticleRegistry())
    , m_particleData(new Particle[m_dataSize])
    , m_colorData(new uint32_t[m_dataSize])
    , m_particleOutOfBounds({ ParticleType::OutOfBounds, 0x00000000 })
    , m_particleVoid({ ParticleType::Void, 0xFF0F0F0F })
{
    std::fill_n(m_particleData.get(), m_dataSize, m_particleVoid);
    std::fill_n(m_colorData.get(), m_dataSize, 0xFF000000);

    m_particleOutOfBounds.isStable = true;

    m_particleData[coordToIndex(Coord{ m_width / 2 , 0 })] = make(ParticleType::Sand);
    m_particleData[coordToIndex(Coord{ m_width / 2 , 1 })] = make(ParticleType::Sand);
    m_particleData[coordToIndex(Coord{ m_width / 2 , 2 })] = make(ParticleType::Sand);
    m_particleData[coordToIndex(Coord{ m_width / 2 , 3 })] = make(ParticleType::Sand);

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

    // Invoke the tick function for each particle.
    for (int i = 0; i < m_dataSize; ++i)
    {
        m_particleData[i].isStable = false;
    }
    std::vector<Coord> unstable;
    //for (int i = 0; i < m_dataSize; ++i)
    for (int i = m_dataSize - 1; i >= 0; --i)
    {
        Particle& particle = m_particleData[i];
        if (particle.lastTickFrame >= m_simulationStep)
        {
            continue;
        }

        particle.lastTickFrame = m_simulationStep;

        Coord postTickCoord = indexToCoord(i);
        TickFunction tickFunction = m_particleRegistry->getTickFunction(particle.type);
        if (tickFunction != nullptr)
        {
            postTickCoord = tickFunction(this, particle, postTickCoord, fixedDeltaTime);
        }

        Particle& postTickParticle = m_particleData[coordToIndex(postTickCoord)];
        if (!postTickParticle.isStable && postTickParticle.type != ParticleType::Void)
        {
            unstable.push_back(postTickCoord);
        }
    }
    m_stabilityPassesThisStep = 0;
    if (!unstable.empty())
    {
        resolveUnstable(unstable, 0, fixedDeltaTime);
    }

    // Copy the color for each particle into the color data array.
    for (int i = 0; i < m_dataSize; ++i)
    {
        m_colorData[i] = m_particleData[i].color;
    }

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    std::cout <<
        "SASI: Particle simulation complete in "
        << ms_int.count() <<
        "ms ("
        << m_stabilityPassesThisStep <<
        " stability passes)\n";

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

bool sasi::ParticleSimulator::isValidCoord(const Coord& coord) const
{
    return (coord.x >= 0) && (coord.x < m_width) && (coord.y >= 0) && (coord.y < m_height);
}

bool sasi::ParticleSimulator::isValidIndex(int index) const
{
    return (index >= 0) && (index < m_dataSize);
}

sasi::Coord sasi::ParticleSimulator::indexToCoord(int index) const
{
    return Coord{ index % m_width, index / m_width };
}

int sasi::ParticleSimulator::coordToIndex(const Coord& coord) const
{
    return (coord.y * m_width) + coord.x;
}

void sasi::ParticleSimulator::setParticle(const Coord& coord, const Particle& particle)
{
    if (!isValidCoord(coord))
    {
        std::cout << "SASI (WARNING): sasi::ParticleSimulator::setParticle was called with an out-of-bounds coord.\n";
        return;
    }

    setParticle(coordToIndex(coord), particle);
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

const sasi::Particle& sasi::ParticleSimulator::getParticle(const Coord& coord) const
{
    if (!isValidCoord(coord))
    {
        return m_particleOutOfBounds;
    }

    return getParticle(coordToIndex(coord));
}

void sasi::ParticleSimulator::makeParticleAtWorldLocation(ParticleType type, const bx::Vec3& worldLocation)
{
    // ASSUMPTION: this object is always at the world origin with the 0,0 particle being top left.

    // Scale the world location using the pixels-per-unit value, trunc-ing any remainder.
    Coord coord{
        static_cast<int>(worldLocation.x * sasi::k_pixelsPerUnit),
        static_cast<int>(worldLocation.y * sasi::k_pixelsPerUnit) };

    if (!isValidCoord(coord))
    {
        std::cout << "SASI (WARNING): makeParticleAtWorldLocation called with out-of-bounds location\n";
        return;
    }

    setParticle(coordToIndex(coord), make(type));
}

void sasi::ParticleSimulator::makeParticlesAlongLine(ParticleType particle, const bx::Vec3& worldStart, const bx::Vec3& worldEnd)
{
    Coord start{
        static_cast<int>(worldStart.x * sasi::k_pixelsPerUnit),
        static_cast<int>(worldStart.y * sasi::k_pixelsPerUnit) };

    Coord end{
        static_cast<int>(worldEnd.x * sasi::k_pixelsPerUnit),
        static_cast<int>(worldEnd.y * sasi::k_pixelsPerUnit) };
    
    std::vector<Coord> line = sasi::getCoordLineBetweenCoords(start, end);
    std::for_each(line.begin(), line.end(), [this, particle](const Coord& coord){
        this->setParticle(coord, this->make(particle));
    });
}

void sasi::ParticleSimulator::move(const Coord& fromCoord, const Coord& toCoord, const Particle& replace)
{
    if ((fromCoord.x == toCoord.x) && (fromCoord.y == toCoord.y))
    {
        return;
    }

    if (!isValidCoord(fromCoord))
    {
        return;
    }

    // Move the particle to the new coord.
    setParticle(toCoord, getParticle(fromCoord));

    // Replace the original particle.
    setParticle(fromCoord, replace);
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

void sasi::ParticleSimulator::swap(const Coord& fromCoord, const Coord& toCoord)
{
    if (!isValidCoord(fromCoord) || !isValidCoord(toCoord))
    {
        return;
    }

    Particle temp = getParticle(fromCoord);
    setParticle(fromCoord, getParticle(toCoord));
    setParticle(toCoord, temp);
}

void sasi::ParticleSimulator::resolveUnstable(const std::vector<sasi::Coord>& unstableCoords, int depth, float fixedDeltaTime)
{
    ++m_stabilityPassesThisStep;

    std::vector<Coord> nextUnstableCoords{};
    auto process = [this, fixedDeltaTime, &nextUnstableCoords, &unstableCoords](const Coord& coord)
    {
        Particle& particle = m_particleData[coordToIndex(coord)];
        TickFunction tickFunction = m_particleRegistry->getTickFunction(particle.type);
        Coord postTickCoord = coord;
        if (tickFunction != nullptr)
        {
            postTickCoord = tickFunction(this, particle, postTickCoord, fixedDeltaTime);
        }
        Particle& postTickParticle = m_particleData[coordToIndex(postTickCoord)];
        if (!postTickParticle.isStable && postTickParticle.type != ParticleType::Void)
        {
            nextUnstableCoords.push_back(postTickCoord);
        }
    };

    if ((depth % 2) != 0)
    {
        for(auto it = unstableCoords.rbegin(); it != unstableCoords.rend(); ++it)
        {
            process(*it);
        }
    }
    else
    {
        for (const Coord& coord : unstableCoords)
        {
            process(coord);
        }
    }

    if (!nextUnstableCoords.empty())
    {
        resolveUnstable(nextUnstableCoords, ++depth, fixedDeltaTime);
    }
}
