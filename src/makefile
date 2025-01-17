CXX = g++

libDir = ../lib
objectDir = ../build/obj
executableDir = ../build

SDLincludeDir = $(libDir)/SDL/include/SDL2
SDLlibraryDir = $(libDir)/SDL/lib

LDFLAGS = -L$(SDLlibraryDir) -Wl,-rpath,$(SDLlibraryDir) -Wl,--enable-new-dtags
LDLIBS = -lSDL2

DEFINES = -DXB_SLOW=1
WARNINGSDISABLED = -Wno-unused-variable -fno-rtti -fno-exceptions -Wno-unused-but-set-variable

INCLUDES = -I$(SDLincludeDir)

SDLCXXFLAGS = -D_REENTRANT
CXXFLAGS = -g -Wall -Werror $(WARNINGSDISABLED) $(DEFINES) $(INCLUDES) $(SDLCXXFLAGS)

dependencies = sdl_xbEngine.h xbEngine.h constants.h

objectFiles = sdl_xbEngine.o xbEngine.o
objects = $(patsubst %,$(objectDir)/%,$(objectFiles))

$(objectDir)/%.o : %.cpp $(dependencies)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

pman : $(objects)
	$(CXX) -o $(executableDir)/$@ $^ $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)
