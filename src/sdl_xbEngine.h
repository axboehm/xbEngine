#ifndef SDL_XBENGINE_H // include guard begin
#define SDL_XBENGINE_H // include guard

#include "xbEngine.h"

#include <stdint.h> // defines fixed size types, C++ version is <cstdint>

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
uint64_t platformGetPerformanceCounter();
float platformGetSecondsElapsed(uint64_t lastCounter, uint64_t thisCounter,
                                uint64_t perfCountFrequency                );

uint32_t platformGetRefreshRate(PlatformWindow *platformWindow);

void platformWait(uint32_t waitTimeMilliSeconds);

PlatformWindow *platformOpenWindow(char *windowTitle,
                                   uint32_t createWidth, uint32_t createHeight);
void platformCloseWindow(PlatformWindow *platformWindow);
void platformGetWindowSize(PlatformWindow *platformWindow, int *width, int *height);
void platformOpenBackBuffer(GameBuffer *gameBuffer);
void platformUpdateBackBuffer(GameBuffer *gameBuffer);
void platformCloseBackBuffer(GameBuffer *gameBuffer);

void platformResizeTexture(PlatformWindow *platformWindow, PlatformTexture *platformTexture,
                           int width, int height                                            );

void platformOpenSoundDevice(uint32_t targetAudioFrameLatency, uint32_t targetRefreshRate,
                             GameSound *gameSound                                         );
void platformCloseSoundDevice();
void platformQueueAudio(GameSound *gameSound);

void platformInitializeControllers(GameInput *gameInput);
void platformResetControllers(GameInput *gameInput);
void platformCloseControllers(GameInput *gameInput);

FileReadResultDEBUG platformReadEntireFileDEBUG(char *fileName);
void platformFreeFileMemoryDEBUG(void *memory);
int32_t platformWriteEntireFileDEBUG(char *fileName, void *memory, uint32_t memorySize);

void platformHandleEvents(GameInput *gameInput, GameGlobal *gameGlobal);

#endif // include guard end
