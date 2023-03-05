#pragma once

#include <fstream>

#include <bgfx/bgfx.h>

bool loadShader(bgfx::ShaderHandle& outShaderHandle, std::string name)
{
    std::ifstream stream;
    stream.open(name);
    if (!stream)
    {
        std::cout << "Failed to open file " << name << "\n";
        return false;
    }

    // Determine file size and read, bgfx frees our allocation.
    stream.seekg(0, std::ios::end);
    size_t fileSize = stream.tellg();
    char* data = new char[fileSize + 1];
    stream.seekg(0, std::ios::beg);
    stream.read(data, fileSize);
    data[fileSize]  = '\0';
    const bgfx::Memory* memory = bgfx::copy(data, fileSize + 1);

    outShaderHandle = bgfx::createShader(memory);
    bgfx::setName(outShaderHandle, name.c_str());
    return true;
}