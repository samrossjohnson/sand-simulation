#pragma once

#include "particle.h"
#include "particle_simulator.h"

sasi::Particle waterMake(const sasi::ParticleSimulator* simulator)
{
    return sasi::Particle{ sasi::ParticleType::Water, 0xFFFF0000 };
}

sasi::Coord waterTick(sasi::ParticleSimulator* simulator, sasi::Particle& particle, const sasi::Coord& coord, float deltaTime)
{
    particle.isStable = true;
    return coord;
    
    /*


    // If the cell below is free, move into it.
    const sasi::Coord& belowCoord{ coord.x, coord.y + 1 };
    const sasi::Particle& below = simulator->getParticle(belowCoord);
    if (below.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-left is free, move into it.
    const sasi::Coord belowLeftCoord{ coord.x - 1, coord.y + 1 };
    const sasi::Particle& belowLeft = simulator->getParticle(belowLeftCoord);
    if (belowLeft.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowLeftCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-right is free, move into it.
    const sasi::Coord belowRightCoord{ coord.x + 1, coord.y + 1 };
    const sasi::Particle& belowRight = simulator->getParticle(belowRightCoord);
    if (belowRight.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowRightCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell left is free spread.
    const sasi::Coord leftCoord{ coord.x - 1, coord.y };
    const sasi::Particle& left = simulator->getParticle(leftCoord);
    if (left.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, leftCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell right is free spread.
    const sasi::Coord rightCoord{ coord.x + 1, coord.y };
    const sasi::Particle& right = simulator->getParticle(rightCoord);
    if (right.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, rightCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    */
}
