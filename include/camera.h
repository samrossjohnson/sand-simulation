#pragma once

#include "sasi.h"

namespace sasi
{
    class Camera
    {
    public:
        Camera(float nearPlane, float farPlane);

        void getView(float* result) const;
        void getproj(float* result, int targetWidth, int targetHeight) const;

        void zoomIn();
        void zoomOut();
        void translate(const bx::Vec3& translation);

        /* Converts a screen (pixel) coordinate into a world space location based on the
           properties of this camera. */
        bx::Vec3 screenToWorldLocation(int width, int height, int x, int y) const;

    private:
        bx::Vec3 m_location;
        float m_zoom;
        float m_nearPlane;
        float m_farPlane;
    };
}
