#include "camera.h"

#define BX_CONFIG_DEBUG 0

#include <bgfx/bgfx.h>
#include <bx/math.h>

sasi::Camera::Camera(float pixelsPerUnit, float nearPlane, float farPlane)
    : k_pixelsPerUnit(pixelsPerUnit)
    , m_zoom(1.0f)
    , m_nearPlane(nearPlane)
    , m_farPlane(farPlane)
{
}

void sasi::Camera::getView(float* result) const
{
    const bx::Vec3 eye{ 0.0f, 0.0f, -1.0f };
    const bx::Vec3 at{ 0.0f, 0.0f, 0.0f };
    const bx::Vec3 up{ 0.0f, 1.0f, 0.0f };
    bx::mtxLookAt(result, eye, at, up);
}

void sasi::Camera::getproj(float* result, int width, int height) const
{
    // NOTES ON CURRENT CAMERA
    // - Pixels-per-unit
    // This is the metric that controls how many pixels (read simulatable particles) fit within 1 game unit. For example
    // a pixels-per-unit of 8 means that 1 square unit contains 64 simulatable pixels.
    //
    // - Zoom
    // Only supports integer values >= 1. Essentially controls how many pixels are equivalent to a particle. For example
    // setting a zoom level of 2 means each particle is rendered with 2x2 pixels.

    const float halfWidth = width / 2.0f / m_zoom;
    const float halfHeight = height / 2.0f / m_zoom;
    bx::mtxOrtho(
        result,
        -halfWidth / k_pixelsPerUnit,
        halfWidth / k_pixelsPerUnit,
        -halfHeight / k_pixelsPerUnit,
        halfHeight / k_pixelsPerUnit,
        m_nearPlane,
        m_farPlane,
        0.0f,
        bgfx::getCaps()->homogeneousDepth);
}
