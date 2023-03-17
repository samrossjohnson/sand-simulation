#include "camera.h"

#include <bgfx/bgfx.h>

sasi::Camera::Camera(float nearPlane, float farPlane)
    : m_zoom(1.0f)
    , m_nearPlane(nearPlane)
    , m_farPlane(farPlane)
    , m_location({ 0.0f, 0.0f, -1.0f })
{
}

void sasi::Camera::getView(float* result) const
{
    const bx::Vec3 at{ m_location.x, m_location.y, 0.0f };
    const bx::Vec3 up{ 0.0f, 1.0f, 0.0f };
    bx::mtxLookAt(result, m_location, at, up);
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

void sasi::Camera::zoomIn()
{
    m_zoom += 1.0f;
}

void sasi::Camera::zoomOut()
{
    m_zoom = bx::max(1.0f, m_zoom - 1.0f);
}

void sasi::Camera::translate(const bx::Vec3& translation)
{
    m_location = bx::add(m_location, translation);
}

bx::Vec3 sasi::Camera::screenToWorldLocation(int width, int height, int x, int y) const
{
    float zoomScalePixPerUnit = sasi::k_pixelsPerUnit * m_zoom;

    // The top-left of the screen is pixel (0, 0). First get that as a
    // world space location.
    const float halfWidth = width / 2.0f;
    const float halfHeight = height / 2.0f;
    const bx::Vec3 screenTopLeftWorldLocation{
        m_location.x - (halfWidth / zoomScalePixPerUnit),
        -m_location.y - (halfHeight / zoomScalePixPerUnit),
        0.0f };

    // The x and y inputs are a pixel offset from the top-left of the screen. Convert that to a world space offset.
    const bx::Vec3 mouseOffsetWorldSpace{ x / (float)zoomScalePixPerUnit, y / (float)zoomScalePixPerUnit, 0.0f };

    const bx::Vec3 screenToWorldLocation = bx::add(screenTopLeftWorldLocation, mouseOffsetWorldSpace);
    return screenToWorldLocation;
}