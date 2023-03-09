#pragma once

#define BX_CONFIG_DEBUG 0

#include <bx/math.h>

namespace sasi
{
    class Camera
    {
    public:
        Camera(float pixelsPerUnit, float nearPlane, float farPlane);

        void getView(float* result) const;
        void getproj(float* result, int targetWidth, int targetHeight) const;

        void zoomIn();
        void zoomOut();
        void translate(const bx::Vec3& translation);

    private:
        const float k_pixelsPerUnit;

        bx::Vec3 m_location;
        float m_zoom;
        float m_nearPlane;
        float m_farPlane;
    };
}
