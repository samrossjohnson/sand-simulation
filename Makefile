# Specify the compiler we want to use.
CC = g++

# Additional compilation options.
# COMPILER_FLAGS =

BGFX_HEADERS =  -Isubmodules/bgfx/include -Isubmodules/bx/include -Isubmodules/bimg/include

# Specify libraries to link against.
LINKER_FLAGS = submodules/bgfx/.build/linux64_gcc/bin/libbgfx-shared-libDebug.so -lSDL2 -lGL -lX11 -ldl -lpthread -lrt

# Target for executable compliation.
all : main.cpp world.cpp particle_simulator.cpp
	./submodules/bgfx/.build/linux64_gcc/bin/shadercDebug \
	-f v_simple.sc \
	-o v_simple.bin \
	-p spirv \
	--platform linux \
	--type vertex \
	--verbose \
	-i submodules/bgfx/src
	./submodules/bgfx/.build/linux64_gcc/bin/shadercDebug \
	-f f_simple.sc \
	-o f_simple.bin \
	-p spirv \
	--platform linux \
	--type fragment \
	--verbose \
	-i submodules/bgfx/src
	$(CC) main.cpp world.cpp particle_simulator.cpp -o main $(LINKER_FLAGS) $(BGFX_HEADERS)
