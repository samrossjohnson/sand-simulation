#pragma once

#include <cstdint>
#include <limits>
#include <vector>

#include "sasi.h"

namespace sasi
{
    enum class ParticleType : uint32_t
    {
        Void = 0,
        OutOfBounds,

        Sand,
        Water,

        Max = std::numeric_limits<uint32_t>::max()
    };

    struct Particle
    {
        Particle()
            : type{ ParticleType::Void }
            , color{ 0x000000 }
            , lastTickFrame{ 0ull }
            , velocityX{ 0.0f }
            , velocityY{ 0.0f }
            , isStable{ false }
        {
        }

        Particle(ParticleType _type, uint32_t _color)
            : type{ _type }
            , color{ _color }
            , lastTickFrame{ 0ull }
            , velocityX{ 0.0f }
            , velocityY{ 0.0f }
            , isStable{ false }
        {
        }

        ParticleType type;
        uint32_t color;
        uint64_t lastTickFrame;
        float velocityX;
        float velocityY;
        bool isStable;
    };

    struct Coord
    {
        Coord()
            : x{ 0 }
            , y{ 0 }
        {
        }

        Coord(int _x, int _y)
            : x{ _x }
            , y{ _y }
        {
        }

        int x;
        int y;
    };

    inline Coord getVelocityAdjustedParticleLocation(const Particle& particle, const Coord& start)
    {   
        // TODO: SJ: Further investigation on impact of cast.
        // We will lose some of the velocity here when we narrow to int, I'm okay with this for now but
        // it could be looked at again.
        return Coord{ 
            start.x + static_cast<int>((particle.velocityX * sasi::k_pixelsPerUnit)),
            start.y + static_cast<int>((particle.velocityY * sasi::k_pixelsPerUnit)) };
    }

    static std::vector<Coord> getCoordLineBetweenCoords(const Coord& a, const Coord& b, bool excludeA = false)
    {
        std::vector<sasi::Coord> coords{};
        int x0 = a.x;
        int y0 = a.y;
        const int x1 = b.x;
        const int y1 = b.y;
        const int dx = std::abs(x1 - x0);
        const int dy = std::abs(y1 - y0);
        const int signX = (x0 < x1) ? 1 : -1;
        const int signY = (y0 < y1) ? 1 : -1;
        int error = dx - dy;
        bool firstIt = true;

        while (true)
        {
            if (!firstIt || (firstIt && !excludeA))
            {
                coords.push_back(Coord{ x0, y0 });
            }

            firstIt = false;

            if ((x0 == x1) && (y0 == y1))
            {
                break;
            }

            int doubleError = error * 2;
            if (doubleError > -dy)
            {
                error -= dy;
                x0 += signX;
            }
            if (doubleError < dx)
            {
                error += dx;
                y0 += signY;
            }
        }

        return coords;
    }
}