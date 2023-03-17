#include "engine/input_state.h"

#include <iostream>

sasi::InputState::InputState()
    : m_keyStates{}
    , m_mouseButtonStates{}
    , m_mouseX { 0 }
    , m_mouseY{ 0 }
{
}

void sasi::InputState::refreshInputStates()
{
    for (auto& it : m_keyStates)
    {
        if (it.second == ButtonState::DownThisFrame)
        {
            it.second = ButtonState::Down;
        }

        else if (it.second == ButtonState::UpThisFrame)
        {
            it.second = ButtonState::Up;
        }
    }

    for (auto& it : m_mouseButtonStates)
    {
        if (it.second == ButtonState::DownThisFrame)
        {
            it.second = ButtonState::Down;
        }

        else if (it.second == ButtonState::UpThisFrame)
        {
            it.second = ButtonState::Up;
        }
    }
}

void sasi::InputState::setKeyDown(SDL_Keycode keyCode)
{
    if (auto it = m_keyStates.find(keyCode); it != m_keyStates.end())
    {
        it->second = (it->second == ButtonState::Up) ? ButtonState::DownThisFrame : ButtonState::Down;
        return;
    }

    m_keyStates.insert(std::make_pair(keyCode, ButtonState::DownThisFrame));
}

void sasi::InputState::setKeyUp(SDL_Keycode keyCode)
{
    if (auto it = m_keyStates.find(keyCode); it != m_keyStates.end())
    {
        it->second = ButtonState::UpThisFrame;
        return;
    }

    m_keyStates.insert(std::make_pair(keyCode, ButtonState::UpThisFrame));
}

void sasi::InputState::setMouseButtonDown(uint8_t mouseButtonIndex)
{
    if (auto it = m_mouseButtonStates.find(mouseButtonIndex); it != m_mouseButtonStates.end())
    {
        it->second = (it->second == ButtonState::Up) ? ButtonState::DownThisFrame : ButtonState::Down;
        return;
    }

    m_mouseButtonStates.insert(std::make_pair(mouseButtonIndex, ButtonState::DownThisFrame));
}

void sasi::InputState::setMouseButtonUp(uint8_t mouseButtonIndex)
{
    if (auto it = m_mouseButtonStates.find(mouseButtonIndex); it != m_mouseButtonStates.end())
    {
        it->second = ButtonState::UpThisFrame;
        return;
    }

    m_mouseButtonStates.insert(std::make_pair(mouseButtonIndex, ButtonState::UpThisFrame));
}

void sasi::InputState::setMouseLocation(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;
}

bool sasi::InputState::isKeyDown(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return (search->second == ButtonState::Down) || (search->second == ButtonState::DownThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isKeyDownThisFrame(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return search->second == ButtonState::DownThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isKeyUp(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return (search->second == ButtonState::Up) || (search->second == ButtonState::UpThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return true;
}

bool sasi::InputState::isKeyUpThisFrame(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return search->second == ButtonState::UpThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isMouseButtonDown(uint8_t mouseButtonIndex) const
{
    if (auto search = m_mouseButtonStates.find(mouseButtonIndex); search != m_mouseButtonStates.end())
    {
        return (search->second == ButtonState::Down) || (search->second == ButtonState::DownThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isMouseButtonDownThisFrame(uint8_t mouseButtonIndex) const
{
    if (auto search = m_mouseButtonStates.find(mouseButtonIndex); search != m_mouseButtonStates.end())
    {
        return search->second == ButtonState::DownThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isMouseButtonUp(uint8_t mouseButtonIndex) const
{
    if (auto search = m_mouseButtonStates.find(mouseButtonIndex); search != m_mouseButtonStates.end())
    {
        return (search->second == ButtonState::Up) || (search->second == ButtonState::UpThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return true;
}

bool sasi::InputState::isMouseButtonUpThisFrame(uint8_t mouseButtonIndex) const
{
    if (auto search = m_mouseButtonStates.find(mouseButtonIndex); search != m_mouseButtonStates.end())
    {
        return search->second == ButtonState::UpThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

void sasi::InputState::getMouseLocation(int& x, int& y) const
{
    x = m_mouseX;
    y = m_mouseY;
}