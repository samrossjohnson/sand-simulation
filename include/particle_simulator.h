#pragma once

#include <memory>
#include <unordered_map>

#include "particle.h"
#include "particle_registry.h"

namespace sasi
{
    class ParticleSimulator
    {
    public:
        ParticleSimulator(int width, int height);
    
        // Move particle registration to a new class `ParticleRegistry`
        // This is forward declared which stops any circular include

        void tick(double fixedDeltaTime);

        size_t getColorDataSize() const;
        int getColorDataWidth() const;
        int getColorDataHeight() const;
        const uint32_t* getColorData() const;

        bool isValidCoord(int x, int y) const;
        bool isValidIndex(int index) const;

        int indexToX(int index) const;
        int indexToY(int index) const;
        int coordToIndex(int x, int y) const;

        void setParticle(int index, const Particle& particle);
        void setParticle(int x, int y, const Particle& particle);

        const Particle& getParticle(int index) const;
        const Particle& getParticle(int x, int y) const;

        // Moves the particle at (fromX, fromY) to (toX, toY) without changing
        // any properties. Original position is filled based on particle passed
        // via replace param.
        void move(int fromX, int fromY, int toX, int toY, const Particle& replace);

        // Invokes the registered make function for a type of particle to create a particle
        // of that type. If no make function was registered, a void particle is returned.
        Particle make(ParticleType type) const;

        // Swaps the particle at (fromX, fromY) with the particle at (toX, toY)
        // without changing any properties. The swap only takes place if both
        // provided particle coordinates are valid.
        void swap(int fromX, int fromY, int toX, int toY);

    private:
        int m_width;
        int m_height;
        int m_dataSize;

        uint64_t m_simulationStep;

        std::unique_ptr<ParticleRegistry> m_particleRegistry;
        std::unique_ptr<Particle[]> m_particleData;
        std::unique_ptr<uint32_t[]> m_colorData;

        Particle m_particleOutOfBounds;
        Particle m_particleVoid;
    };
}