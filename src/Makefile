CXX = g++

objectDir = ../build/obj
executableDir = ../build

SDLincludeDir = /usr/include/SDL2

LDLIBS = -lSDL2

DEFINES = -DXB_SLOW=1
WARNINGSDISABLED = -Wno-unused-variable -fno-rtti -fno-exceptions -Wno-unused-but-set-variable

INCLUDES = -I$(SDLincludeDir)

SDLCompileFlags = -D_REENTRANT
CompileFlags = -g -Wall -Werror $(WARNINGSDISABLED) $(DEFINES) $(SDLCompileFlags) $(INCLUDES)

dependencies = platform_xbEngine.h xbEngine.h xbMath.h constants.h

objectFiles = sdl_xbEngine.o xbEngine.o
objects = $(patsubst %,$(objectDir)/%,$(objectFiles))

$(objectDir)/%.o : %.cpp $(dependencies)
	@mkdir -p $(objectDir)
	$(CXX) -c -o $@ $< $(CompileFlags)

xbEngine : $(objects)
	$(CXX) -o $(executableDir)/$@ $^ $(LDLIBS)

clean :
	rm -f $(objectDir)/*.o $(executableDir)/xbEngine
