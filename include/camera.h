#pragma once

namespace sasi
{
    class Camera
    {
    public:
        Camera(float pixelsPerUnit, float nearPlane, float farPlane);

        void getView(float* result) const;
        void getproj(float* result, int targetWidth, int targetHeight) const;

    private:
        const float k_pixelsPerUnit;

        float m_zoom;
        float m_nearPlane;
        float m_farPlane;
    };
}
