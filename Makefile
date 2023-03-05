# Specify the compiler we want to use.
CC = g++

BGFX_HEADERS =  -Isubmodules/bgfx/include -Isubmodules/bx/include -Isubmodules/bimg/include

SASI_HEADERS = -Iinclude

# Specify libraries to link against.
LINKER_FLAGS = submodules/bgfx/.build/linux64_gcc/bin/libbgfx-shared-libDebug.so -lSDL2 -lGL -lX11 -ldl -lpthread -lrt

# Make bin structure.
bin :
	mkdir bin
	mkdir bin/shaders

# Target for executable compliation.
all : bin src/main.cpp src/world.cpp src/particle_simulator.cpp src/particle_registry.cpp
	./submodules/bgfx/.build/linux64_gcc/bin/shadercDebug \
	-f shaders/v_simple.sc \
	-o bin/shaders/v_simple.bin \
	-p spirv \
	--platform linux \
	--type vertex \
	--verbose \
	-i submodules/bgfx/src
	./submodules/bgfx/.build/linux64_gcc/bin/shadercDebug \
	-f shaders/f_simple.sc \
	-o bin/shaders/f_simple.bin \
	-p spirv \
	--platform linux \
	--type fragment \
	--verbose \
	-i submodules/bgfx/src
	$(CC) src/main.cpp src/world.cpp src/particle_simulator.cpp src/particle_registry.cpp -o bin/main $(LINKER_FLAGS) $(BGFX_HEADERS) $(SASI_HEADERS)
