#include <memory>
#include <unordered_map>

#include "particle.h"

namespace sasi
{
    class ParticleSimulator
    {
    public:
        typedef void (*TickFunction)(ParticleSimulator* simulator, Particle& particle, int x, int y);
        typedef Particle (*MakeFunction)(const ParticleSimulator* simulator);

        ParticleSimulator(int width, int height);

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

        std::unique_ptr<Particle[]> m_particleData;
        std::unique_ptr<uint32_t[]> m_colorData;
        std::unordered_map<ParticleType, TickFunction> m_tickFunctions;
        std::unordered_map<ParticleType, MakeFunction> m_makeFunctions;

        Particle m_particleOutOfBounds;
        Particle m_particleVoid;
    };
}