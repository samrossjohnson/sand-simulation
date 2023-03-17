#pragma once

#include <unordered_map>

#include <SDL2/SDL_keycode.h>

namespace sasi
{
    class InputState
    {
    private:
        enum class ButtonState
        {
            Down = 0,
            DownThisFrame,
            Up,
            UpThisFrame
        };

    public:
        InputState();

        void refreshInputStates();
        void setKeyDown(SDL_Keycode keyCode);
        void setKeyUp(SDL_Keycode keyCode);
        void setMouseButtonDown(uint8_t mouseButtonIndex);
        void setMouseButtonUp(uint8_t mouseButtonIndex);
        void setMouseLocation(int x, int y);

        bool isKeyDown(SDL_Keycode keyCode) const;
        bool isKeyDownThisFrame(SDL_Keycode keyCode) const;
        bool isKeyUp(SDL_Keycode keyCode) const;
        bool isKeyUpThisFrame(SDL_Keycode keyCode) const;

        bool isMouseButtonDown(uint8_t mouseButtonIndex) const;
        bool isMouseButtonDownThisFrame(uint8_t mouseButtonIndex) const;
        bool isMouseButtonUp(uint8_t mouseButtonIndex) const;
        bool isMouseButtonUpThisFrame(uint8_t mouseButtonIndex) const;

        void getMouseLocation(int& x, int& y) const;

    private:
        std::unordered_map<SDL_Keycode, ButtonState> m_keyStates;
        std::unordered_map<uint8_t, ButtonState> m_mouseButtonStates;
        int m_mouseX;
        int m_mouseY;
    };
}