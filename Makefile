# Specify the compiler we want to use.
CC = g++

# Additional compilation options.
# COMPILER_FLAGS =

BGFX_HEADERS =  -Isubmodules/bgfx/include -Isubmodules/bx/include -Isubmodules/bimg/include

# Specify libraries to link against.
LINKER_FLAGS = submodules/bgfx/.build/linux64_gcc/bin/libbgfx-shared-libDebug.so -lSDL2 -lGL -lX11 -ldl -lpthread -lrt

# Target for executable compliation.
all : main.cpp
	$(CC) main.cpp -o main $(LINKER_FLAGS) $(BGFX_HEADERS)