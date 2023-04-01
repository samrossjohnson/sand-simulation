#pragma once

#include <memory>
#include <unordered_map>

#include "sasi.h"

#include "particle.h"
#include "particle_registry.h"

namespace sasi
{
    class ParticleSimulator
    {
    public:
        ParticleSimulator(int width, int height);

        void tick(double fixedDeltaTime);

        size_t getColorDataSize() const;
        int getColorDataWidth() const;
        int getColorDataHeight() const;
        const uint32_t* getColorData() const;

        bool isValidCoord(const Coord& coord) const;
        bool isValidIndex(int index) const;

        Coord indexToCoord(int index) const;
        int coordToIndex(const Coord& coord) const;

        void setParticle(int index, const Particle& particle);
        void setParticle(const Coord& coord, const Particle& particle);

        const Particle& getParticle(int index) const;
        const Particle& getParticle(const Coord& coord) const;

        // Creates a particle at the world location given. If the world location corresponds to an
        // a cell that is outside the bounds of a simulation the particle will not be created.
        void makeParticleAtWorldLocation(ParticleType type, const bx::Vec3& worldLocation);

        // Makes a line of particles along the line worldStart -> worldEnd. Particles will not be made for
        // any parts of the line that are outside of the simulation bounds.
        void makeParticlesAlongLine(ParticleType, const bx::Vec3& worldStart, const bx::Vec3& worldEnd);

        // Moves the particle at fromCoord to toCoord without changing
        // any properties. Original position is filled based on particle passed
        // via replace param.
        void move(const Coord& fromCoord, const Coord& toCoord, const Particle& replace);

        // Invokes the registered make function for a type of particle to create a particle
        // of that type. If no make function was registered, a void particle is returned.
        Particle make(ParticleType type) const;

        // Swaps the particle at fromCoord with the particle at toCoord
        // without changing any properties. The swap only takes place if both
        // provided particle coordinates are valid.
        void swap(const Coord& fromCoord, const Coord& toCoord);

    private:
        void resolveUnstable(const std::vector<Coord>& unstableCoords, int depth, float fixedDeltaTime);

    private:
        int m_width;
        int m_height;
        int m_dataSize;

        uint64_t m_simulationStep;
        int m_stabilityPassesThisStep;
        
        std::unique_ptr<ParticleRegistry> m_particleRegistry;
        std::unique_ptr<Particle[]> m_particleData;
        std::unique_ptr<uint32_t[]> m_colorData;

        Particle m_particleOutOfBounds;
        Particle m_particleVoid;
    };
}