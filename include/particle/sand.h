#pragma once

#include "particle.h"
#include "particle_simulator.h"

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
