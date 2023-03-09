#pragma once

#include <unordered_map>

#include <SDL2/SDL_keycode.h>

namespace sasi
{
    class InputState
    {
    private:
        enum class KeyState
        {
            Down = 0,
            DownThisFrame,
            Up,
            UpThisFrame
        };

    public:
        InputState();

        void refreshKeyStates();
        void setKeyDown(SDL_Keycode keyCode);
        void setKeyUp(SDL_Keycode keyCode);

        bool isKeyDown(SDL_Keycode keyCode) const;
        bool isKeyDownThisFrame(SDL_Keycode keyCode) const;
        bool isKeyUp(SDL_Keycode keyCode) const;
        bool isKeyUpThisFrame(SDL_Keycode keyCode) const;

    private:
        std::unordered_map<SDL_Keycode, KeyState> m_keyStates;
    };
}