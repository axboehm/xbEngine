#include "constants.h"
#include "xbEngine.h"
#include "sdl_xbEngine.h"

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_events.h>
#include <SDL_gamecontroller.h>
// #include <cassert> // for assert macro - use xbAssert from constants.h
#include <cstdio> // for printf
#include <cstring> // for memset
// #include <thread> // for multithreading and sleeping the thread
// #include <chrono> // for sleep
#include <immintrin.h> // for __rdtsc (should work on all x86 compilers)

//NOTE[ALEX]: platform dependent code should stay in this file,
//            all other files should be independent of the platform

struct PlatformWindow {
    SDL_Window   *window;
    SDL_Renderer *renderer;
};

struct PlatformTexture {
    SDL_Texture *textureHandle;
};

void platformInit()
{
    int sdlInitCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
    if (sdlInitCode != 0) {
        printf("%s SDL_Init failed with code %i\n", __FUNCTION__, sdlInitCode);
    }
}

void platformInitClocks(GameClocks *gameClocks)
{
    gameClocks->perfCountFrequency = SDL_GetPerformanceFrequency();
    platformGetClocks(gameClocks);
}

uint64_t platformGetPerformanceCounter()
{
    return SDL_GetPerformanceCounter();
}

void platformGetClocks(GameClocks *gameClocks)
{
    gameClocks->endPerfCounter     = platformGetPerformanceCounter();
    gameClocks->elapsedPerfCounter = gameClocks->endPerfCounter - gameClocks->lastPerfCounter;
    gameClocks->lastPerfCounter    = gameClocks->endPerfCounter;

    gameClocks->msLastFrame =   (float)(gameClocks->elapsedPerfCounter)
                              / (float)(gameClocks->perfCountFrequency);
    // printf("Final: %0.4fms/f, %0.2ff/s   |   CPU only: %0.4fms/f, %0.2ff/s\n",
    //        gameClocks->msLastFrame, 1.0f/gameClocks->msLastFrame,
    //        gameClocks->msLastFrameCPU, 1.0f/gameClocks->msLastFrameCPU        );

    gameClocks->endCycleCount     = __rdtsc();
    gameClocks->elapsedCycleCount = gameClocks->endCycleCount - gameClocks->lastCycleCount;
    gameClocks->lastCycleCount    = gameClocks->endCycleCount;
}

void platformGetElapsedCPU(GameClocks *gameClocks)
{
    uint64_t elapsed = platformGetPerformanceCounter() - gameClocks->lastPerfCounter;
    gameClocks->msLastFrameCPU = (float)elapsed / (float)(gameClocks->perfCountFrequency);
}

float platformGetSecondsElapsed(uint64_t lastCounter, uint64_t thisCounter,
                                uint64_t perfCountFrequency                )
{
    return ((float)(thisCounter - lastCounter) / (float)perfCountFrequency);
}

uint32_t platformGetRefreshRate(PlatformWindow *platformWindow)
{
    uint32_t result = 0;
    SDL_DisplayMode displayMode;
    int displayIndex = SDL_GetWindowDisplayIndex(platformWindow->window);
    if (SDL_GetDesktopDisplayMode(displayIndex, &displayMode) == 0) {
        result = (uint32_t)displayMode.refresh_rate;
    }
    printf("%s displayIndex: %i, refresh rate: %u\n", __FUNCTION__, displayIndex, result);
    return result;
}

void platformWait(uint32_t waitTimeMilliSeconds)
{
    uint32_t timeToSleep = waitTimeMilliSeconds - 1; //NOTE[ALEX]: occasionally, the wait call is
                                                     //            not granular enough,
                                                     //            so add some slack here
    // printf("%s sleeping %ums\n", __FUNCTION__, timeToSleep);
    SDL_Delay(timeToSleep);
}

PlatformWindow *platformOpenWindow(char *windowTitle, uint32_t createWidth, uint32_t createHeight)
{
    PlatformWindow *platformWindow = (PlatformWindow *)malloc(sizeof(PlatformWindow));
    platformWindow->window   = 0;
    platformWindow->renderer = 0;

    platformWindow->window = SDL_CreateWindow(windowTitle,
                                              SDL_WINDOWPOS_UNDEFINED,
                                              SDL_WINDOWPOS_UNDEFINED,
                                              (int)createWidth, (int)createHeight,
                                              SDL_WINDOW_RESIZABLE                );
    if (!platformWindow->window) {
        printf("%s SDL_CreateWindow failed\n", __FUNCTION__);
    } else {
        platformWindow->renderer = SDL_CreateRenderer(platformWindow->window, -1, 0);
        if (!platformWindow->renderer) {
            printf("%s SDL_CreateRenderer failed\n", __FUNCTION__);
        }
    }
    
    return platformWindow;
}

void platformCloseWindow(PlatformWindow *platformWindow)
{
    if (platformWindow->window) {
        SDL_DestroyWindow(platformWindow->window);
    } else {
        printf("%s no window to destroy\n", __FUNCTION__);
    }
    if (platformWindow->renderer) {
        SDL_DestroyRenderer(platformWindow->renderer);
    } else {
        printf("%s no renderer to destroy\n", __FUNCTION__);
    }
    
    SDL_Quit();

    if (platformWindow) {
        free(platformWindow);
    } else {
        printf("%s no memory to free\n", __FUNCTION__);
    }
}

void platformCloseBuffer(GameBuffer *gameBuffer)
{
    if (gameBuffer->backBuffer.textureMemory) {
        free(gameBuffer->backBuffer.textureMemory); 
    } else {
        printf("gameGlobal->testTexture.textureMemory nothing to free\n");
    }
    if (gameBuffer->backBuffer.memory) {
        free(gameBuffer->backBuffer.memory); 
    } else {
        printf("gameGlobal->testTexture.memory nothing to free\n");
    }
}

void platformGetWindowSize(PlatformWindow *platformWindow, int *width, int *height)
{
    SDL_GetWindowSize(platformWindow->window, width, height);
}

void platformResizeTexture(PlatformWindow *platformWindow, PlatformTexture *platformTexture,
                           GameTexture *gameTexture                                         )
{
    if (platformTexture->textureHandle != 0) {
        SDL_DestroyTexture(platformTexture->textureHandle); 
    }
    if (gameTexture->textureMemory) { 
        free(gameTexture->textureMemory); 
    }

    if (platformWindow->renderer) {
        platformTexture->textureHandle = SDL_CreateTexture(platformWindow->renderer,
                                                           SDL_PIXELFORMAT_ARGB8888,
                                                           SDL_TEXTUREACCESS_STREAMING,
                                                           gameTexture->width,
                                                           gameTexture->height         );
        gameTexture->textureMemory = malloc(  gameTexture->bytesPerPixel
                                            * gameTexture->width * gameTexture->height);
    } else {
        printf("%s renderer not initialized\n", __FUNCTION__);
    }
}

void platformOpenSoundDevice(uint32_t targetAudioFrameLatency, GameSound *gameSound)
{
    xbAssert(targetAudioFrameLatency > 0);

    SDL_AudioSpec sdlAudioSettings = {};
    sdlAudioSettings.freq     = AUDIO_SAMPLES_PER_SECOND;
    sdlAudioSettings.format   = AUDIO_S16LSB; // signed 16bit little endian for each sample
    sdlAudioSettings.channels = AUDIO_CHANNELS;
    sdlAudioSettings.samples  = AUDIO_SAMPLES_PER_CALL; // buffer size per channel,
                                                        // this will be passed per call,
                                                        // so this is the minimum audio latency
    sdlAudioSettings.callback = 0; // if left empty, SDL_QueueAudio can be used
    
    SDL_OpenAudio(&sdlAudioSettings, 0); //NOTE[ALEX]: passing 0 to the output will make this
                                         //            modify the passed input instead

    if (sdlAudioSettings.format != AUDIO_S16LSB) {
        printf("%s format is not what was requested, but %u.\n",
               __FUNCTION__, sdlAudioSettings.format            );
    }
    gameSound->bytesPerSamplePerChannel = sizeof(int16_t);
    float targetLatency = (1.0f/60.0f) * (float)targetAudioFrameLatency;
    gameSound->targetQueuedBytes =
        (uint32_t)( targetLatency
                   *(float)(AUDIO_SAMPLES_PER_SECOND*gameSound->bytesPerSamplePerChannel));

    SDL_PauseAudio(0); // 0 for unpause, 1 for pause
}

void platformCloseSoundDevice()
{
    SDL_CloseAudio(); //NOTE[ALEX]: also included in SDL_Quit();
}

void platformQueueAudio(GameSound *gameSound, int16_t *audioToQueue, uint32_t audioToQueueBytes)
{
    if (audioToQueueBytes) {
        SDL_QueueAudio(1, audioToQueue, audioToQueueBytes);
    }
    gameSound->queuedBytes = SDL_GetQueuedAudioSize(1);
    // printf("%s, %u\n", __FUNCTION__, gameSound->queuedBytes);
}

inline uint32_t safeTruncateUInt64(uint64_t value)
{
    xbAssert(value <= 0xFFFFFFFF);
    return (uint32_t)value;
}

FileReadResultDEBUG platformReadEntireFileDEBUG(char *fileName)
{
    FileReadResultDEBUG fileReadResult = {};
    fileReadResult.contents    = 0;
    fileReadResult.contentSize = 0;

    SDL_RWops *file = SDL_RWFromFile(fileName, "rb"); // open a file in binary read-only mode

    if (file != 0) {
        uint32_t fileSize = safeTruncateUInt64(file->size(file));
        if (fileSize > 0) {
            fileReadResult.contents = (void *)malloc(fileSize);
            if (fileReadResult.contents) {
                if (SDL_RWread(file, fileReadResult.contents, fileSize, 1)) {
                    fileReadResult.contentSize = fileSize;
                    printf("%s read successful, bytes read: %u\n", __FUNCTION__, fileSize);
                } else {
                    platformFreeFileMemoryDEBUG(fileReadResult.contents);
                    printf("%s read failed\n", __FUNCTION__);
                }
            } else {
                printf("%s allocation failed\n", __FUNCTION__);
            }
        } else {
            printf("%s file empty\n", __FUNCTION__);
        }
        SDL_RWclose(file);
    } else {
        printf("%s file handle NULL\n", __FUNCTION__);
    }

    return fileReadResult;
}

void platformFreeFileMemoryDEBUG(void *memory)
{
    if (memory) {
        free(memory);
        printf("%s free successful\n", __FUNCTION__);
    } else {
        printf("%s memory NULL\n", __FUNCTION__);
    }
}

int32_t platformWriteEntireFileDEBUG(char *fileName, void *memory, uint32_t memorySize)
{
    int32_t result = 0;

    SDL_RWops *file = SDL_RWFromFile(fileName, "wb"); // (over)write a file in binary write-only mode
    
    if (file != NULL) {
        if (SDL_RWwrite(file, memory, memorySize, 1)) { // returns number of objects written (here 1)
            uint32_t fileSize = file->size(file);
            result = (int32_t)(fileSize == memorySize);
            printf("%s write successful, bytes given: %u, bytes written: %u\n",
                   __FUNCTION__, memorySize, fileSize                          );
        } else {
            result = 0;
            printf("%s write failed\n", __FUNCTION__);
        }
        SDL_RWclose(file);
    } else {
        result = 0;
        printf("%s file handle NULL\n", __FUNCTION__);
    }

    return result;
}

//NOTE[ALEX]: OS key repeat will not trigger as a new key press,
//            interpreting a held down key is the responsibility of the engine
void buttonStateUpdateDown(ButtonState *buttonState)
{
    //NOTE[ALEX]: key repeat of OS will trigger this call even while the key is held down
    if (buttonState->isDown == 0) {
        buttonState->isDown = 1;
        buttonState->transitionCount += 1;
    }
}

void SDL2MouseButtonDown(SDL_Event mouseButtonEvent, GameInput *gameInput)
{
    switch(mouseButtonEvent.button.button) {
        case SDL_BUTTON_LEFT:   buttonStateUpdateDown(&gameInput->m1); break;
        case SDL_BUTTON_MIDDLE: buttonStateUpdateDown(&gameInput->m2); break;
        case SDL_BUTTON_RIGHT:  buttonStateUpdateDown(&gameInput->m3); break;
        case SDL_BUTTON_X1:     buttonStateUpdateDown(&gameInput->m4); break;
        case SDL_BUTTON_X2:     buttonStateUpdateDown(&gameInput->m5); break;
    }
}

void SDL2KeyboardKeyDown(SDL_Event keyEvent, GameInput *gameInput)
{
    switch (keyEvent.key.keysym.sym) {
        case SDLK_a: buttonStateUpdateDown(&gameInput->a); break;
        case SDLK_b: buttonStateUpdateDown(&gameInput->b); break;
        case SDLK_c: buttonStateUpdateDown(&gameInput->c); break;
        case SDLK_d: buttonStateUpdateDown(&gameInput->d); break;
        case SDLK_e: buttonStateUpdateDown(&gameInput->e); break;
        case SDLK_f: buttonStateUpdateDown(&gameInput->f); break;
        case SDLK_g: buttonStateUpdateDown(&gameInput->g); break;
        case SDLK_h: buttonStateUpdateDown(&gameInput->h); break;
        case SDLK_i: buttonStateUpdateDown(&gameInput->i); break;
        case SDLK_j: buttonStateUpdateDown(&gameInput->j); break;
        case SDLK_k: buttonStateUpdateDown(&gameInput->k); break;
        case SDLK_l: buttonStateUpdateDown(&gameInput->l); break;
        case SDLK_m: buttonStateUpdateDown(&gameInput->m); break;
        case SDLK_n: buttonStateUpdateDown(&gameInput->n); break;
        case SDLK_o: buttonStateUpdateDown(&gameInput->o); break;
        case SDLK_p: buttonStateUpdateDown(&gameInput->p); break;
        case SDLK_q: buttonStateUpdateDown(&gameInput->q); break;
        case SDLK_r: buttonStateUpdateDown(&gameInput->r); break;
        case SDLK_s: buttonStateUpdateDown(&gameInput->s); break;
        case SDLK_t: buttonStateUpdateDown(&gameInput->t); break;
        case SDLK_u: buttonStateUpdateDown(&gameInput->u); break;
        case SDLK_v: buttonStateUpdateDown(&gameInput->v); break;
        case SDLK_w: buttonStateUpdateDown(&gameInput->w); break;
        case SDLK_x: buttonStateUpdateDown(&gameInput->x); break;
        case SDLK_y: buttonStateUpdateDown(&gameInput->y); break;
        case SDLK_z: buttonStateUpdateDown(&gameInput->z); break;

        case SDLK_BACKQUOTE:    buttonStateUpdateDown((&gameInput->backQuote));    break;
        case SDLK_MINUS:        buttonStateUpdateDown((&gameInput->minus));        break;
        case SDLK_EQUALS:       buttonStateUpdateDown((&gameInput->equals));       break;
        case SDLK_LEFTBRACKET:  buttonStateUpdateDown((&gameInput->leftBracket));  break;
        case SDLK_RIGHTBRACKET: buttonStateUpdateDown((&gameInput->rightBracket)); break;
        case SDLK_SEMICOLON:    buttonStateUpdateDown((&gameInput->semicolon));    break;
        case SDLK_QUOTE:        buttonStateUpdateDown((&gameInput->quote));        break;
        case SDLK_COMMA:        buttonStateUpdateDown((&gameInput->comma));        break;
        case SDLK_PERIOD:       buttonStateUpdateDown((&gameInput->period));       break;
        case SDLK_SLASH:        buttonStateUpdateDown((&gameInput->slash));        break;

        case SDLK_1: buttonStateUpdateDown(&gameInput->one);   break;
        case SDLK_2: buttonStateUpdateDown(&gameInput->two);   break;
        case SDLK_3: buttonStateUpdateDown(&gameInput->three); break;
        case SDLK_4: buttonStateUpdateDown(&gameInput->four);  break;
        case SDLK_5: buttonStateUpdateDown(&gameInput->five);  break;
        case SDLK_6: buttonStateUpdateDown(&gameInput->six);   break;
        case SDLK_7: buttonStateUpdateDown(&gameInput->seven); break;
        case SDLK_8: buttonStateUpdateDown(&gameInput->eight); break;
        case SDLK_9: buttonStateUpdateDown(&gameInput->nine);  break;
        case SDLK_0: buttonStateUpdateDown(&gameInput->zero);  break;

        case SDLK_ESCAPE:    buttonStateUpdateDown(&gameInput->esc);       break;
        case SDLK_TAB:       buttonStateUpdateDown(&gameInput->tab);       break;
        case SDLK_CAPSLOCK:  buttonStateUpdateDown(&gameInput->caps);      break;
        case SDLK_BACKSPACE: buttonStateUpdateDown(&gameInput->backspace); break;
        case SDLK_BACKSLASH: buttonStateUpdateDown(&gameInput->backslash); break;
        case SDLK_RETURN:    buttonStateUpdateDown(&gameInput->enter);     break;

        case SDLK_SPACE: buttonStateUpdateDown(&gameInput->space); break;

        case SDLK_LCTRL:  buttonStateUpdateDown(&gameInput->lCtrl);  break;
        case SDLK_RCTRL:  buttonStateUpdateDown(&gameInput->rCtrl);  break;
        case SDLK_LSHIFT: buttonStateUpdateDown(&gameInput->lShift); break;
        case SDLK_RSHIFT: buttonStateUpdateDown(&gameInput->rShift); break;
        case SDLK_LALT:   buttonStateUpdateDown(&gameInput->lAlt);   break;
        case SDLK_RALT:   buttonStateUpdateDown(&gameInput->rAlt);   break;

        case SDLK_INSERT:   buttonStateUpdateDown(&gameInput->insert);   break;
        case SDLK_HOME:     buttonStateUpdateDown(&gameInput->home);     break;
        case SDLK_PAGEUP:   buttonStateUpdateDown(&gameInput->pageUp);   break;
        case SDLK_DELETE:   buttonStateUpdateDown(&gameInput->del);      break;
        case SDLK_END:      buttonStateUpdateDown(&gameInput->end);      break;
        case SDLK_PAGEDOWN: buttonStateUpdateDown(&gameInput->pageDown); break;
        
        case SDLK_UP:    buttonStateUpdateDown(&gameInput->up);    break;
        case SDLK_DOWN:  buttonStateUpdateDown(&gameInput->down);  break;
        case SDLK_LEFT:  buttonStateUpdateDown(&gameInput->left);  break;
        case SDLK_RIGHT: buttonStateUpdateDown(&gameInput->right); break;

        case SDLK_F1:  buttonStateUpdateDown(&gameInput->f1);  break;
        case SDLK_F2:  buttonStateUpdateDown(&gameInput->f2);  break;
        case SDLK_F3:  buttonStateUpdateDown(&gameInput->f3);  break;
        case SDLK_F4:  buttonStateUpdateDown(&gameInput->f4);  break;
        case SDLK_F5:  buttonStateUpdateDown(&gameInput->f5);  break;
        case SDLK_F6:  buttonStateUpdateDown(&gameInput->f6);  break;
        case SDLK_F7:  buttonStateUpdateDown(&gameInput->f7);  break;
        case SDLK_F8:  buttonStateUpdateDown(&gameInput->f8);  break;
        case SDLK_F9:  buttonStateUpdateDown(&gameInput->f9);  break;
        case SDLK_F10: buttonStateUpdateDown(&gameInput->f10); break;
        case SDLK_F11: buttonStateUpdateDown(&gameInput->f11); break;
        case SDLK_F12: buttonStateUpdateDown(&gameInput->f12); break;

        case SDLK_PRINTSCREEN: buttonStateUpdateDown(&gameInput->printScreen); break;
        case SDLK_SCROLLLOCK:  buttonStateUpdateDown(&gameInput->scrollLock);  break;
        case SDLK_PAUSE:       buttonStateUpdateDown(&gameInput->pause);       break;

        case SDLK_NUMLOCKCLEAR: buttonStateUpdateDown(&gameInput->numLock);     break;
        case SDLK_KP_DIVIDE:    buttonStateUpdateDown(&gameInput->numDivide);   break;
        case SDLK_KP_MULTIPLY:  buttonStateUpdateDown(&gameInput->numMultiply); break;
        case SDLK_KP_MINUS:     buttonStateUpdateDown(&gameInput->numMinus);    break;
        case SDLK_KP_PLUS:      buttonStateUpdateDown(&gameInput->numPlus);     break;
        case SDLK_KP_ENTER:     buttonStateUpdateDown(&gameInput->numEnter);    break;
        case SDLK_KP_PERIOD:    buttonStateUpdateDown(&gameInput->numPeriod);   break;
        case SDLK_KP_1:         buttonStateUpdateDown(&gameInput->numOne);      break;
        case SDLK_KP_2:         buttonStateUpdateDown(&gameInput->numTwo);      break;
        case SDLK_KP_3:         buttonStateUpdateDown(&gameInput->numThree);    break;
        case SDLK_KP_4:         buttonStateUpdateDown(&gameInput->numFour);     break;
        case SDLK_KP_5:         buttonStateUpdateDown(&gameInput->numFive);     break;
        case SDLK_KP_6:         buttonStateUpdateDown(&gameInput->numSix);      break;
        case SDLK_KP_7:         buttonStateUpdateDown(&gameInput->numSeven);    break;
        case SDLK_KP_8:         buttonStateUpdateDown(&gameInput->numEight);    break;
        case SDLK_KP_9:         buttonStateUpdateDown(&gameInput->numNine);     break;
    }
}

void buttonStateUpdateUp(ButtonState *buttonState)
{
    xbAssert(buttonState->isDown == 1); // there is no keyrepeat for letting go of a key
    buttonState->isDown = 0;
    buttonState->transitionCount += 1;
}

void SDL2MouseButtonUp(SDL_Event mouseButtonEvent, GameInput *gameInput)
{
    switch(mouseButtonEvent.button.button) {
        case SDL_BUTTON_LEFT:   buttonStateUpdateUp(&gameInput->m1); break;
        case SDL_BUTTON_MIDDLE: buttonStateUpdateUp(&gameInput->m2); break;
        case SDL_BUTTON_RIGHT:  buttonStateUpdateUp(&gameInput->m3); break;
        case SDL_BUTTON_X1:     buttonStateUpdateUp(&gameInput->m4); break;
        case SDL_BUTTON_X2:     buttonStateUpdateUp(&gameInput->m5); break;
    }
}

void SDL2KeyboardKeyUp(SDL_Event keyEvent, GameInput *gameInput)
{
    switch (keyEvent.key.keysym.sym) {
        case SDLK_a: buttonStateUpdateUp(&gameInput->a); break;
        case SDLK_b: buttonStateUpdateUp(&gameInput->b); break;
        case SDLK_c: buttonStateUpdateUp(&gameInput->c); break;
        case SDLK_d: buttonStateUpdateUp(&gameInput->d); break;
        case SDLK_e: buttonStateUpdateUp(&gameInput->e); break;
        case SDLK_f: buttonStateUpdateUp(&gameInput->f); break;
        case SDLK_g: buttonStateUpdateUp(&gameInput->g); break;
        case SDLK_h: buttonStateUpdateUp(&gameInput->h); break;
        case SDLK_i: buttonStateUpdateUp(&gameInput->i); break;
        case SDLK_j: buttonStateUpdateUp(&gameInput->j); break;
        case SDLK_k: buttonStateUpdateUp(&gameInput->k); break;
        case SDLK_l: buttonStateUpdateUp(&gameInput->l); break;
        case SDLK_m: buttonStateUpdateUp(&gameInput->m); break;
        case SDLK_n: buttonStateUpdateUp(&gameInput->n); break;
        case SDLK_o: buttonStateUpdateUp(&gameInput->o); break;
        case SDLK_p: buttonStateUpdateUp(&gameInput->p); break;
        case SDLK_q: buttonStateUpdateUp(&gameInput->q); break;
        case SDLK_r: buttonStateUpdateUp(&gameInput->r); break;
        case SDLK_s: buttonStateUpdateUp(&gameInput->s); break;
        case SDLK_t: buttonStateUpdateUp(&gameInput->t); break;
        case SDLK_u: buttonStateUpdateUp(&gameInput->u); break;
        case SDLK_v: buttonStateUpdateUp(&gameInput->v); break;
        case SDLK_w: buttonStateUpdateUp(&gameInput->w); break;
        case SDLK_x: buttonStateUpdateUp(&gameInput->x); break;
        case SDLK_y: buttonStateUpdateUp(&gameInput->y); break;
        case SDLK_z: buttonStateUpdateUp(&gameInput->z); break;

        case SDLK_BACKQUOTE:    buttonStateUpdateUp((&gameInput->backQuote));    break;
        case SDLK_MINUS:        buttonStateUpdateUp((&gameInput->minus));        break;
        case SDLK_EQUALS:       buttonStateUpdateUp((&gameInput->equals));       break;
        case SDLK_LEFTBRACKET:  buttonStateUpdateUp((&gameInput->leftBracket));  break;
        case SDLK_RIGHTBRACKET: buttonStateUpdateUp((&gameInput->rightBracket)); break;
        case SDLK_SEMICOLON:    buttonStateUpdateUp((&gameInput->semicolon));    break;
        case SDLK_QUOTE:        buttonStateUpdateUp((&gameInput->quote));        break;
        case SDLK_COMMA:        buttonStateUpdateUp((&gameInput->comma));        break;
        case SDLK_PERIOD:       buttonStateUpdateUp((&gameInput->period));       break;
        case SDLK_SLASH:        buttonStateUpdateUp((&gameInput->slash));        break;

        case SDLK_1: buttonStateUpdateUp(&gameInput->one);   break;
        case SDLK_2: buttonStateUpdateUp(&gameInput->two);   break;
        case SDLK_3: buttonStateUpdateUp(&gameInput->three); break;
        case SDLK_4: buttonStateUpdateUp(&gameInput->four);  break;
        case SDLK_5: buttonStateUpdateUp(&gameInput->five);  break;
        case SDLK_6: buttonStateUpdateUp(&gameInput->six);   break;
        case SDLK_7: buttonStateUpdateUp(&gameInput->seven); break;
        case SDLK_8: buttonStateUpdateUp(&gameInput->eight); break;
        case SDLK_9: buttonStateUpdateUp(&gameInput->nine);  break;
        case SDLK_0: buttonStateUpdateUp(&gameInput->zero);  break;

        case SDLK_ESCAPE:    buttonStateUpdateUp(&gameInput->esc);       break;
        case SDLK_TAB:       buttonStateUpdateUp(&gameInput->tab);       break;
        case SDLK_CAPSLOCK:  buttonStateUpdateUp(&gameInput->caps);      break;
        case SDLK_BACKSPACE: buttonStateUpdateUp(&gameInput->backspace); break;
        case SDLK_BACKSLASH: buttonStateUpdateUp(&gameInput->backslash); break;
        case SDLK_RETURN:    buttonStateUpdateUp(&gameInput->enter);     break;

        case SDLK_SPACE: buttonStateUpdateUp(&gameInput->space); break;

        case SDLK_LCTRL:  buttonStateUpdateUp(&gameInput->lCtrl);  break;
        case SDLK_RCTRL:  buttonStateUpdateUp(&gameInput->rCtrl);  break;
        case SDLK_LSHIFT: buttonStateUpdateUp(&gameInput->lShift); break;
        case SDLK_RSHIFT: buttonStateUpdateUp(&gameInput->rShift); break;
        case SDLK_LALT:   buttonStateUpdateUp(&gameInput->lAlt);   break;
        case SDLK_RALT:   buttonStateUpdateUp(&gameInput->rAlt);   break;

        case SDLK_INSERT:   buttonStateUpdateUp(&gameInput->insert);   break;
        case SDLK_HOME:     buttonStateUpdateUp(&gameInput->home);     break;
        case SDLK_PAGEUP:   buttonStateUpdateUp(&gameInput->pageUp);   break;
        case SDLK_DELETE:   buttonStateUpdateUp(&gameInput->del);      break;
        case SDLK_END:      buttonStateUpdateUp(&gameInput->end);      break;
        case SDLK_PAGEDOWN: buttonStateUpdateUp(&gameInput->pageDown); break;
        
        case SDLK_UP:    buttonStateUpdateUp(&gameInput->up);    break;
        case SDLK_DOWN:  buttonStateUpdateUp(&gameInput->down);  break;
        case SDLK_LEFT:  buttonStateUpdateUp(&gameInput->left);  break;
        case SDLK_RIGHT: buttonStateUpdateUp(&gameInput->right); break;

        case SDLK_F1:  buttonStateUpdateUp(&gameInput->f1);  break;
        case SDLK_F2:  buttonStateUpdateUp(&gameInput->f2);  break;
        case SDLK_F3:  buttonStateUpdateUp(&gameInput->f3);  break;
        case SDLK_F4:  buttonStateUpdateUp(&gameInput->f4);  break;
        case SDLK_F5:  buttonStateUpdateUp(&gameInput->f5);  break;
        case SDLK_F6:  buttonStateUpdateUp(&gameInput->f6);  break;
        case SDLK_F7:  buttonStateUpdateUp(&gameInput->f7);  break;
        case SDLK_F8:  buttonStateUpdateUp(&gameInput->f8);  break;
        case SDLK_F9:  buttonStateUpdateUp(&gameInput->f9);  break;
        case SDLK_F10: buttonStateUpdateUp(&gameInput->f10); break;
        case SDLK_F11: buttonStateUpdateUp(&gameInput->f11); break;
        case SDLK_F12: buttonStateUpdateUp(&gameInput->f12); break;

        case SDLK_PRINTSCREEN: buttonStateUpdateUp(&gameInput->printScreen); break;
        case SDLK_SCROLLLOCK:  buttonStateUpdateUp(&gameInput->scrollLock);  break;
        case SDLK_PAUSE:       buttonStateUpdateUp(&gameInput->pause);       break;

        case SDLK_NUMLOCKCLEAR: buttonStateUpdateUp(&gameInput->numLock);     break;
        case SDLK_KP_DIVIDE:    buttonStateUpdateUp(&gameInput->numDivide);   break;
        case SDLK_KP_MULTIPLY:  buttonStateUpdateUp(&gameInput->numMultiply); break;
        case SDLK_KP_MINUS:     buttonStateUpdateUp(&gameInput->numMinus);    break;
        case SDLK_KP_PLUS:      buttonStateUpdateUp(&gameInput->numPlus);     break;
        case SDLK_KP_ENTER:     buttonStateUpdateUp(&gameInput->numEnter);    break;
        case SDLK_KP_PERIOD:    buttonStateUpdateUp(&gameInput->numPeriod);   break;
        case SDLK_KP_1:         buttonStateUpdateUp(&gameInput->numOne);      break;
        case SDLK_KP_2:         buttonStateUpdateUp(&gameInput->numTwo);      break;
        case SDLK_KP_3:         buttonStateUpdateUp(&gameInput->numThree);    break;
        case SDLK_KP_4:         buttonStateUpdateUp(&gameInput->numFour);     break;
        case SDLK_KP_5:         buttonStateUpdateUp(&gameInput->numFive);     break;
        case SDLK_KP_6:         buttonStateUpdateUp(&gameInput->numSix);      break;
        case SDLK_KP_7:         buttonStateUpdateUp(&gameInput->numSeven);    break;
        case SDLK_KP_8:         buttonStateUpdateUp(&gameInput->numEight);    break;
        case SDLK_KP_9:         buttonStateUpdateUp(&gameInput->numNine);     break;
    }
}

struct PlatformController {
    SDL_GameController *controllerHandle;
    SDL_JoystickID      sdlID; //NOTE[ALEX]: every time a controller gets plugged in, it gets a
                               //            new ID, use this to reference where to store input
                               //            -1 is an invalid ID and should be the initialized value
};

int32_t SDL2FindControllerID(GameInput *gameInput, SDL_JoystickID joystickID)
{
    for (int32_t i = 0; i < MAX_CONTROLLERS; i++) {
        if (!gameInput->controllerConnected[i]) { continue; }
        PlatformController *platformController =
            (PlatformController *)(gameInput->platformControllers[i]);
        if (joystickID == platformController->sdlID) {
            return i;
        }
    }
    printf("%s Could not find corresponding controller %i.\n", __FUNCTION__, joystickID);
    return -1;
}

void SDL2ControllerButtonDown(SDL_Event cButtonEvent, GameInput *gameInput,
                              ControllerInput *controllerInput             )
{
    int32_t controllerID = SDL2FindControllerID(gameInput, cButtonEvent.cbutton.which);
    if (controllerID == -1) { return; }

    switch (cButtonEvent.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A:             
            buttonStateUpdateDown(&controllerInput[controllerID].fDown); break;
        case SDL_CONTROLLER_BUTTON_B:             
            buttonStateUpdateDown(&controllerInput[controllerID].fRight); break;
        case SDL_CONTROLLER_BUTTON_X:             
            buttonStateUpdateDown(&controllerInput[controllerID].fLeft); break;
        case SDL_CONTROLLER_BUTTON_Y:             
            buttonStateUpdateDown(&controllerInput[controllerID].fUp); break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:       
            buttonStateUpdateDown(&controllerInput[controllerID].dUp); break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     
            buttonStateUpdateDown(&controllerInput[controllerID].dDown); break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     
            buttonStateUpdateDown(&controllerInput[controllerID].dLeft); break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    
            buttonStateUpdateDown(&controllerInput[controllerID].dRight); break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  
            buttonStateUpdateDown(&controllerInput[controllerID].shoulderLeft); break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: 
            buttonStateUpdateDown(&controllerInput[controllerID].shoulderRight); break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:     
            buttonStateUpdateDown(&controllerInput[controllerID].stickClickLeft); break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    
            buttonStateUpdateDown(&controllerInput[controllerID].stickClickRight); break;
        case SDL_CONTROLLER_BUTTON_BACK:          
            buttonStateUpdateDown(&controllerInput[controllerID].select); break;
        case SDL_CONTROLLER_BUTTON_GUIDE:         
            buttonStateUpdateDown(&controllerInput[controllerID].guide); break;
        case SDL_CONTROLLER_BUTTON_START:         
            buttonStateUpdateDown(&controllerInput[controllerID].start); break;
        case SDL_CONTROLLER_BUTTON_MISC1:         
            buttonStateUpdateDown(&controllerInput[controllerID].misc); break;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD:      
            buttonStateUpdateDown(&controllerInput[controllerID].touch); break;
    }
}

void SDL2ControllerButtonUp(SDL_Event cButtonEvent, GameInput *gameInput,
                            ControllerInput *controllerInput             )
{
    int32_t controllerID = SDL2FindControllerID(gameInput, cButtonEvent.cbutton.which);
    if (controllerID == -1) { return; }

    switch (cButtonEvent.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A:             
            buttonStateUpdateUp(&controllerInput[controllerID].fDown); break;
        case SDL_CONTROLLER_BUTTON_B:             
            buttonStateUpdateUp(&controllerInput[controllerID].fRight); break;
        case SDL_CONTROLLER_BUTTON_X:             
            buttonStateUpdateUp(&controllerInput[controllerID].fLeft); break;
        case SDL_CONTROLLER_BUTTON_Y:             
            buttonStateUpdateUp(&controllerInput[controllerID].fUp); break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:       
            buttonStateUpdateUp(&controllerInput[controllerID].dUp); break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     
            buttonStateUpdateUp(&controllerInput[controllerID].dDown); break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     
            buttonStateUpdateUp(&controllerInput[controllerID].dLeft); break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    
            buttonStateUpdateUp(&controllerInput[controllerID].dRight); break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  
            buttonStateUpdateUp(&controllerInput[controllerID].shoulderLeft); break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: 
            buttonStateUpdateUp(&controllerInput[controllerID].shoulderRight); break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:     
            buttonStateUpdateUp(&controllerInput[controllerID].stickClickLeft); break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    
            buttonStateUpdateUp(&controllerInput[controllerID].stickClickRight); break;
        case SDL_CONTROLLER_BUTTON_BACK:          
            buttonStateUpdateUp(&controllerInput[controllerID].select); break;
        case SDL_CONTROLLER_BUTTON_GUIDE:         
            buttonStateUpdateUp(&controllerInput[controllerID].guide); break;
        case SDL_CONTROLLER_BUTTON_START:         
            buttonStateUpdateUp(&controllerInput[controllerID].start); break;
        case SDL_CONTROLLER_BUTTON_MISC1:         
            buttonStateUpdateUp(&controllerInput[controllerID].misc); break;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD:      
            buttonStateUpdateUp(&controllerInput[controllerID].touch); break;
    }
}

void SDL2ControllerAxisMotion(SDL_Event cAxisEvent, GameInput *gameInput,
                              ControllerInput *controllerInput           )
{
    int16_t axisValue = cAxisEvent.caxis.value;
    if (axisValue >= -CONTR_AXIS_DEADZONE_INNER && axisValue <= CONTR_AXIS_DEADZONE_INNER) {
        axisValue = 0;
    } else {
        if (axisValue < -CONTR_AXIS_DEADZONE_OUTER) {
            axisValue = -CONTR_AXIS_DEADZONE_OUTER;
        } else if (axisValue > CONTR_AXIS_DEADZONE_OUTER) {
            axisValue = CONTR_AXIS_DEADZONE_OUTER;
        }
        int16_t offsetZero = CONTR_AXIS_DEADZONE_INNER;
        if (axisValue < 0) { offsetZero *= -1; }
        float axisValueNormalized  = (float)(axisValue - offsetZero);
              axisValueNormalized /= (float)(CONTR_AXIS_DEADZONE_OUTER - CONTR_AXIS_DEADZONE_INNER);
              axisValueNormalized *= (float)CONTR_AXIS_NORMALIZATION;
        axisValue = (int16_t)axisValueNormalized;
    }

    int32_t controllerID = SDL2FindControllerID(gameInput, cAxisEvent.caxis.which);
    if (controllerID == -1) { return; }

    //NOTE[ALEX]: pushing a stick forward returns a negative value, so the y axes are inverted
    switch (cAxisEvent.caxis.axis) {
        case SDL_CONTROLLER_AXIS_LEFTX:
            controllerInput[controllerID].leftStickX = axisValue; break;
        case SDL_CONTROLLER_AXIS_LEFTY:
            controllerInput[controllerID].leftStickY = -axisValue; break;
        case SDL_CONTROLLER_AXIS_RIGHTX:
            controllerInput[controllerID].rightStickX = axisValue; break;
        case SDL_CONTROLLER_AXIS_RIGHTY:
            controllerInput[controllerID].rightStickY = -axisValue; break;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            controllerInput[controllerID].leftTrigger = axisValue; break;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            controllerInput[controllerID].rightTrigger = axisValue; break;
    }
}

//TODO[ALEX]: replugging moves index up

void platformInitializeControllers(GameInput *gameInput)
{
    for (uint32_t i = 0; i < MAX_CONTROLLERS; i++) {
        if (!gameInput->platformControllers[i]) {
            PlatformController *platformController =
                (PlatformController *)malloc(sizeof(PlatformController));
            platformController->controllerHandle = 0;
            platformController->sdlID = -1;
            gameInput->platformControllers[i] = platformController;
        } else {
            printf("%s controller already initialized.\n", __FUNCTION__);
        }
    }
}

void platformCloseControllers(GameInput *gameInput)
{
    for (uint32_t i = 0; i < MAX_CONTROLLERS; i++) {
        if (gameInput->platformControllers[i]) {
            PlatformController *platformController =
                (PlatformController *)(gameInput->platformControllers[i]);
            if (platformController->controllerHandle) {
                SDL_GameControllerClose(platformController->controllerHandle);
                printf("Closed Game Controller %u.\n", i);
            }
            free(gameInput->platformControllers[i]);
        } else {
            printf("gameInput->platformControllers[%u] nothing to free.\n", i);
        }
    }
}

void platformResetControllers(GameInput *gameInput)
{
    for (uint32_t i = 0; i < MAX_CONTROLLERS; i++) { // close all existing controllers
        gameInput->controllerConnected[i] = 0;
        if (gameInput->platformControllers[i]) {
            PlatformController *platformController =
                (PlatformController *)(gameInput->platformControllers[i]);
            if (platformController->controllerHandle) {
                SDL_GameControllerClose(platformController->controllerHandle);
                platformController->controllerHandle = 0;
                platformController->sdlID = -1;
                printf("Closed Game Controller %u.\n", i);
            }
        } else {
            printf("%s platformController %u not initialized.\n", __FUNCTION__, i);
        }
    }

    uint32_t maxControllers  = SDL_NumJoysticks();
    printf("maxControllers: %u\n", maxControllers);
    uint32_t controllerIndex = 0;
    for (uint32_t i = 0; i < maxControllers; i++) { // open all connected controllers
        if (!SDL_IsGameController(i)) { continue; }
        if (controllerIndex >= MAX_CONTROLLERS) {
            printf("%s too many controllers plugged in.\n", __FUNCTION__);
            break;
        }
        gameInput->controllerConnected[i] = 1;
        PlatformController *platformController =
            (PlatformController *)(gameInput->platformControllers[controllerIndex]);
        platformController->controllerHandle = SDL_GameControllerOpen(i);
        SDL_Joystick* joystick =
            SDL_GameControllerGetJoystick(platformController->controllerHandle);
        platformController->sdlID = SDL_JoystickInstanceID(joystick);

        // gameInput->platformControllers[controllerIndex]->sdlID = 
        printf("Opened Game Controller %u to controllerIndex %u with sdlID %i.\n",
               i, controllerIndex, platformController->sdlID                      );
        controllerIndex++;
    }
}

void platformHandleEvents(GameBuffer *gameBuffer, GameInput *gameInput, GameGlobal *gameGlobal)
{
    // cleanup from previous frame to prevent inputs sticking
    gameInput->mouseScrH = 0;
    gameInput->mouseScrV = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                gameGlobal->quitGame = 1;
            } break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED: {
                        gameGlobal->stopRendering = true;
                    } break;
                    case SDL_WINDOWEVENT_RESTORED: {
                        gameGlobal->stopRendering = false;
                    } break;
                    case SDL_WINDOWEVENT_RESIZED: {
                        // printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n",
                        //        event->window.data1, event->window.data2);
                        platformGetWindowSize((PlatformWindow *)(gameBuffer->memory),
                                              &gameBuffer->backBuffer.width,
                                              &gameBuffer->backBuffer.height       );
                        platformResizeTexture((PlatformWindow *)(gameBuffer->memory),
                                              (PlatformTexture *)(gameBuffer->backBuffer.memory),
                                              &gameBuffer->backBuffer                            );
                    } break;
                    case SDL_WINDOWEVENT_EXPOSED: {
                    } break;
                }
            } break;
            case SDL_AUDIODEVICEADDED: {
            } break;
            case SDL_AUDIODEVICEREMOVED: {
            } break;
            case SDL_KEYDOWN: {
                SDL2KeyboardKeyDown(event, gameInput);
            } break;
            case SDL_KEYUP: {
                SDL2KeyboardKeyUp(event, gameInput);
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                SDL2MouseButtonDown(event, gameInput);
            } break;
            case SDL_MOUSEBUTTONUP: {
                SDL2MouseButtonUp(event, gameInput);
            } break;
            case SDL_MOUSEWHEEL: {
                gameInput->mouseScrH = event.wheel.x;
                gameInput->mouseScrV = event.wheel.y;
            } break;
            case SDL_MOUSEMOTION: { // position of mouse pixel pos in window
                gameInput->mousePosX = event.motion.x;
                gameInput->mousePosY = event.motion.y;
            } break;
            case SDL_CONTROLLERDEVICEADDED: {
                platformResetControllers(gameInput);
            } break;
            case SDL_CONTROLLERDEVICEREMOVED: {
                platformResetControllers(gameInput);
            } break;
            case SDL_CONTROLLERDEVICEREMAPPED: {
            } break;
            case SDL_CONTROLLERBUTTONDOWN: {
                SDL2ControllerButtonDown(event, gameInput, gameInput->controller);
            } break;
            case SDL_CONTROLLERBUTTONUP: {
                SDL2ControllerButtonUp(event, gameInput, gameInput->controller);
            } break;
            case SDL_CONTROLLERAXISMOTION: {
                SDL2ControllerAxisMotion(event, gameInput, gameInput->controller);
            } break;
            case SDL_JOYBUTTONDOWN: { // controllers trigger both this and controllerbuttondown
            } break;
            case SDL_JOYBUTTONUP: {
            } break;
            case SDL_JOYAXISMOTION: {
            } break;
            case SDL_CONTROLLERTOUCHPADMOTION: {
            } break;
            case SDL_CONTROLLERTOUCHPADUP: { // also handled in controllerbuttonup
            } break;
            case SDL_CONTROLLERTOUCHPADDOWN: {
            } break;
            case SDL_TEXTINPUT: {
                //NOTE[ALEX]: while running from the terminal, every key press also
                //            sends an event of this type
            } break;
            default: {
                printf("SDL_Event %x unhandled\n", event.type);
            } break;
        }
    }
}

void platformUpdateWindow(PlatformWindow *platformWindow, PlatformTexture *platformTexture,
                          int width, int height, uint32_t bytesPerPixel, void *textureMemory)
//    GameBuffer *gameBuffer, GameTexture *gameTexture) {
{

    SDL_UpdateTexture(platformTexture->textureHandle, 0, textureMemory, width * bytesPerPixel);
    SDL_RenderCopy(platformWindow->renderer, platformTexture->textureHandle, 0, 0);
    SDL_RenderPresent(platformWindow->renderer);
}

int main(int argc, char **argv)
{
    // INITIALIZATION
    GameMemory gameMemory = {};
    gameMemory.permanentMemSize = Megabytes(64);
    gameMemory.permanentMem     = (uint64_t *)malloc(gameMemory.permanentMemSize);
    gameMemory.transientMemSize = Gigabytes(1);
    gameMemory.transientMem     = (uint64_t *)malloc(gameMemory.transientMemSize);
    // all memory is pre initialized to 0
    if (gameMemory.transientMem && gameMemory.permanentMem) {
        memset(gameMemory.permanentMem, 0, gameMemory.permanentMemSize);
        memset(gameMemory.transientMem, 0, gameMemory.transientMemSize);
        gameMemory.initialized =  1;
    } else {
        printf("Could not allocated gameMemory: transient: %lu, permanent: %lu\n",
               (uint64_t)gameMemory.transientMem, (uint64_t)gameMemory.permanentMem);
        return 0;
    }

    xbAssert(gameMemory.initialized);

    printf("size of gameMemory.permanentMemSize: %lu\n", gameMemory.permanentMemSize);
    printf("size of gameMemory.transientMemSize: %lu\n", gameMemory.transientMemSize);
    printf("size of gameState: %lu (includes subsequent)\n", sizeof(GameState));
    printf("size of gameGlobal: %lu\n", sizeof(GameGlobal));
    printf("size of gameInput: %lu\n", sizeof(GameInput));
    printf("size of gameClocks: %lu\n", sizeof(GameClocks));
    printf("size of gameBuffer: %lu\n", sizeof(GameBuffer));
    printf("size of gameSound: %lu\n", sizeof(GameSound));

    xbAssert(sizeof(GameState) <= gameMemory.permanentMemSize);

    GameState  *gameState  = (GameState *)gameMemory.permanentMem;
    GameGlobal *gameGlobal = &gameState->gameGlobal;
    GameInput  *gameInput  = &gameState->gameInput;
    GameClocks *gameClocks = &gameState->gameClocks;
    GameBuffer *gameBuffer = &gameState->gameBuffer;
    GameSound  *gameSound  = &gameState->gameSound;

    xbAssert(   &gameInput->terminatorMouse - &gameInput->mButtons[0]
             == sizeof(gameInput->mButtons)/sizeof(gameInput->mButtons[0]));
    xbAssert(   &gameInput->terminatorKeys - &gameInput->keys[0]
             == sizeof(gameInput->keys)/sizeof(gameInput->keys[0]));
    xbAssert(   &gameInput->controller[0].terminatorContr - &gameInput->controller[0].buttons[0]
             == sizeof(gameInput->controller[0].buttons)/sizeof(gameInput->controller[0].buttons[0]));

    platformInit();
    gameBuffer->memory = platformOpenWindow((char *)WINDOW_TITLE,
                                            WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT    );
    platformInitClocks(gameClocks);
    gameGlobal->monitorRefreshRate = platformGetRefreshRate((PlatformWindow *)(gameBuffer->memory));
    if (gameGlobal->monitorRefreshRate == 0) { //NOTE[ALEX]: temporary
        gameGlobal->renderingRefreshRate = 60;
    } else {
        gameGlobal->renderingRefreshRate = gameGlobal->monitorRefreshRate;
    }
    gameGlobal->targetTimePerFrame = 1000.0f / (float)(gameGlobal->renderingRefreshRate);
    uint32_t targetAudioFrameLatency = 4;
    platformOpenSoundDevice(targetAudioFrameLatency, gameSound);
    platformInitializeControllers(gameInput);

    gameBuffer->backBuffer.width         = 1024;
    gameBuffer->backBuffer.height        = 1024;
    gameBuffer->backBuffer.bytesPerPixel = 4;
    PlatformTexture *platformTexture = (PlatformTexture *)malloc(sizeof(PlatformTexture));
    platformTexture->textureHandle = 0;
    gameBuffer->backBuffer.memory = platformTexture;
    platformGetWindowSize((PlatformWindow *)(gameBuffer->memory),
                          &gameBuffer->backBuffer.width, &gameBuffer->backBuffer.height);
    platformResizeTexture((PlatformWindow *)(gameBuffer->memory),
                          (PlatformTexture *)(gameBuffer->backBuffer.memory),
                          &gameBuffer->backBuffer                            );

    //audio test
    gameGlobal->toneHz = 256;
    gameGlobal->toneVolume = 500;
    gameGlobal->wavePeriod = AUDIO_SAMPLES_PER_SECOND / gameGlobal->toneHz;
    gameGlobal->halfWavePeriod = gameGlobal->wavePeriod / 2;

    // MAIN LOOP
    while (!gameGlobal->quitGame) {
        platformHandleEvents(gameBuffer, gameInput, gameGlobal);

        if (gameGlobal->stopRendering) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(MINIMIZED_SLEEP_TIME));
            //TODO[ALEX]: use SDL_Delay
            continue;
        }

        gameUpdate(gameState, &gameMemory);
        platformQueueAudio(gameSound, gameSound->audioToQueue, gameSound->audioToQueueBytes);

        platformUpdateWindow((PlatformWindow *)(gameBuffer->memory),
                             (PlatformTexture *)(gameBuffer->backBuffer.memory),
                             gameBuffer->backBuffer.width, gameBuffer->backBuffer.height,
                             gameBuffer->backBuffer.bytesPerPixel,
                             gameBuffer->backBuffer.textureMemory                        );

        platformGetElapsedCPU(gameClocks);

        float mSecondsElapsed = 1000.0f* platformGetSecondsElapsed(gameClocks->lastPerfCounter,
                                                          platformGetPerformanceCounter(),
                                                          gameClocks->perfCountFrequency  );
        int32_t timeToSleep = (gameGlobal->targetTimePerFrame - mSecondsElapsed);
        if (timeToSleep > 0) {
            platformWait(timeToSleep);
            //NOTE[ALEX]: to bridge the "gap" introduced by the lower granularity of the wait call,
            //            stay in the following while loop until the target frame time is reached
            while (1000.0f * platformGetSecondsElapsed(gameClocks->lastPerfCounter,
                                                       platformGetPerformanceCounter(),
                                                       gameClocks->perfCountFrequency  )
                   < gameGlobal->targetTimePerFrame                                     ) { }
        }

        platformGetClocks(gameClocks);
    }

    // CLEANUP
    platformCloseControllers(gameInput);
    platformCloseSoundDevice();
    platformCloseWindow((PlatformWindow *)gameBuffer->memory);
    platformCloseBuffer(gameBuffer);

    free(gameMemory.transientMem);
    free(gameMemory.permanentMem);

    return 0;
}
