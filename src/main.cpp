#include <memory>

#include "engine/engine.h"

int main(int argc, char* argv[])
{
    std::unique_ptr<sasi::Engine> engine{ new sasi::Engine{ 600, 400 } };
    if (engine.get() == nullptr)
    {
        return -1;
    }
    if (!engine->isInitialized())
    {
        return -1;
    }

    bool exit = false;
    while (!exit)
    {
        exit = engine->tick();
    }

    return 0;
}
