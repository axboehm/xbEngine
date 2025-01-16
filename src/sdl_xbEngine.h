#ifndef SDL_XBENGINE_H // include guard begin
#define SDL_XBENGINE_H // include guard

#include "xbEngine.h"

//NOTE[ALEX]: platform dependent definitions should stay in this file,
//            all other files should be independent of the platform,
//            use void * to point to PlatformStructs

struct PlatformWindow;
struct PlatformTexture;
struct PlatformController;

struct FileReadResultDEBUG {
    uint32_t  contentSize;
    void     *contents;
};

void platformInit();

void platformInitClocks(GameClocks *gameClocks);
void platformGetClocks(GameClocks *gameClocks);

PlatformWindow *platformOpenWindow(char *windowTitle,
                                   uint32_t createWidth, uint32_t createHeight);
void platformCloseWindow(PlatformWindow *platformWindow);
void platformGetWindowSize(PlatformWindow *platformWindow, int *width, int *height);
void platformCloseBuffer(GameBuffer *gameBuffer);

void platformResizeTexture(PlatformWindow *platformWindow, PlatformTexture *platformTexture,
                           GameTexture *gameTexture                                         );

void platformOpenSoundDevice(float targetLatency, GameSound *gameSound);
void platformCloseSoundDevice();
void platformQueueAudio(GameSound *gameSound);

void platformInitializeControllers(GameInput *gameInput);
void platformResetControllers(GameInput *gameInput);
void platformCloseControllers(GameInput *gameInput);

FileReadResultDEBUG platformReadEntireFileDEBUG(char *fileName);
void platformFreeFileMemoryDEBUG(void *memory);
int32_t platformWriteEntireFileDEBUG(char *fileName, void *memory, uint64_t memorySize);

void platformHandleEvents(GameInput *gameInput, GameGlobal *gameGlobal);

#endif // include guard end
