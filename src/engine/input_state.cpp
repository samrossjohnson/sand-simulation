#include "engine/input_state.h"

sasi::InputState::InputState()
    : m_keyStates({})
{
}

void sasi::InputState::refreshKeyStates()
{
    for (auto& it : m_keyStates)
    {
        if (it.second == KeyState::DownThisFrame)
        {
            it.second = KeyState::Down;
        }

        else if (it.second == KeyState::UpThisFrame)
        {
            it.second = KeyState::Up;
        }
    }
}

void sasi::InputState::setKeyDown(SDL_Keycode keyCode)
{
    if (auto it = m_keyStates.find(keyCode); it != m_keyStates.end())
    {
        it->second = (it->second == KeyState::Up) ? KeyState::DownThisFrame : KeyState::Down;
        return;
    }

    m_keyStates.insert(std::make_pair(keyCode, KeyState::DownThisFrame));
}

void sasi::InputState::setKeyUp(SDL_Keycode keyCode)
{
    if (auto it = m_keyStates.find(keyCode); it != m_keyStates.end())
    {
        it->second = KeyState::UpThisFrame;
        return;
    }

    m_keyStates.insert(std::make_pair(keyCode, KeyState::UpThisFrame));
}

bool sasi::InputState::isKeyDown(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return (search->second == KeyState::Down) || (search->second == KeyState::DownThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isKeyDownThisFrame(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return search->second == KeyState::DownThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}

bool sasi::InputState::isKeyUp(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return (search->second == KeyState::Up) || (search->second == KeyState::UpThisFrame);
    }

    // No entry we have never met the key and assume it's up.
    return true;
}

bool sasi::InputState::isKeyUpThisFrame(SDL_Keycode keyCode) const
{
    if (auto search = m_keyStates.find(keyCode); search != m_keyStates.end())
    {
        return search->second == KeyState::UpThisFrame;
    }

    // No entry we have never met the key and assume it's up.
    return false;
}
