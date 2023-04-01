#pragma once

#include "sasi.h"

#include "particle.h"
#include "particle_simulator.h"

#include <vector>
#include <cmath>
#include <iostream>

sasi::Particle sandMake(const sasi::ParticleSimulator* simulator)
{
    return sasi::Particle{ sasi::ParticleType::Sand, 0xFF00FFFF };
}

sasi::Coord sandTick(sasi::ParticleSimulator* simulator, sasi::Particle& particle, const sasi::Coord& coord, float deltaTime)
{
    particle.velocityY += sasi::k_gravity * deltaTime;

    sasi::Coord oldCoord = coord;
    sasi::Coord endCoord = sasi::getVelocityAdjustedParticleLocation(particle, coord);
    if (oldCoord.x == endCoord.x && oldCoord.y == endCoord.y)
    {
        // TODO: there is a crash if we continue past here with the same coord we start with
        return coord;
    }
    std::vector<sasi::Coord> lineCoords = sasi::getCoordLineBetweenCoords(coord, endCoord, true);
    sasi::Coord hitCoord = coord;
    for (const sasi::Coord& lineCoord : lineCoords)
    {
        hitCoord = lineCoord;
        const sasi::Particle& testParticle = simulator->getParticle(lineCoord);
        if (testParticle.type != sasi::ParticleType::Void)
        {
            break;
        }

        // Now we will know the particle we impacted. If we hit nothing we can move there, else we check if the thing
        // we hit is stable. If it's a stable impact we can impact it for real, else we need to yield to the stability passes.  
    }
    
    const sasi::Particle& hitParticle = simulator->getParticle(hitCoord);
    if (hitParticle.type == sasi::ParticleType::Void)
    {
        particle.isStable = true;
        simulator->move(coord, hitCoord, simulator->make(sasi::ParticleType::Void));
        return hitCoord;
    }
    else if (hitParticle.isStable)
    {
        sasi::Coord aboveHit{ hitCoord.x, hitCoord.y - 1 };
        particle.velocityX = 0.0f;
        particle.velocityY = hitParticle.velocityY;
        particle.isStable = true;
        simulator->move(coord, aboveHit, simulator->make(sasi::ParticleType::Void));
        return aboveHit;
    }

    particle.velocityY -= sasi::k_gravity * deltaTime;
    particle.isStable = false;
    return coord;

    //// WORKING NON-VEL CODE
    /*
    const sasi::Coord belowCoord{ coord.x, coord.y + 1 };
    const sasi::Particle& belowParticle = simulator->getParticle(belowCoord);
    
    if (belowParticle.type == sasi::ParticleType::Void)
    {
        particle.isStable = true;
        simulator->move(coord, belowCoord, simulator->make(sasi::ParticleType::Void));
        return belowCoord;
    }

    if (belowParticle.isStable)
    {
        particle.isStable = true;
        return coord;
    }

    particle.isStable = false;
    return coord;
    */
    //// END OF ORKING NON-VEL CODE


    /*
    const sasi::Coord belowCoord{ coord.x, coord.y + 1 };
    const sasi::Particle& below = simulator->getParticle(belowCoord);
    if (below.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }
    */

    /*
    return;

    particle.velocityY += sasi::k_gravity * deltaTime;

    sasi::Coord oldCoord = coord;
    sasi::Coord endCoord = sasi::getVelocityAdjustedParticleLocation(particle, coord);
    std::vector<sasi::Coord> lineCoords = sasi::getCoordLineBetweenCoords(coord, endCoord, true);
    for (const sasi::Coord& lineCoord : lineCoords)
    {
        const sasi::Particle& below = simulator->getParticle(lineCoord);
        if (below.type == sasi::ParticleType::Void)
        {
            simulator->move(oldCoord, lineCoord, simulator->make(sasi::ParticleType::Void));
            oldCoord = lineCoord;
        }
        else
        {
            particle.velocityX = 0.0f;
            particle.velocityY = 0.0f;
            break;
        }
    }


    return;

    // Get the coordinate that the particle wants to be in according to it's velocity.
    // Apply method along line of particles from start to end.

    // If the cell below is free, move into it.
    const sasi::Coord belowCoord{ coord.x, coord.y + 1 };
    const sasi::Particle& below = simulator->getParticle(belowCoord);
    if (below.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the below type is a liquid we can move through, swap us.
    if (below.type == sasi::ParticleType::Water)
    {
       simulator->swap(coord, belowCoord);
    }

    // If the cell below-left is free, move into it.
    const sasi::Coord& belowLeftCoord{ coord.x - 1, coord.y + 1 };
    const sasi::Particle& belowLeft = simulator->getParticle(belowLeftCoord);
    if (belowLeft.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowLeftCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }

    // If the cell below-right is free, move into it.
    const sasi::Coord& belowRightCoord{ coord.x + 1, coord.y + 1 };
    const sasi::Particle& belowRight = simulator->getParticle(belowRightCoord);
    if (belowRight.type == sasi::ParticleType::Void)
    {
        simulator->move(coord, belowRightCoord, simulator->make(sasi::ParticleType::Void));
        return;
    }  
    */
}
