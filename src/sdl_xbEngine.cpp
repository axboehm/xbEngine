#include "constants.h"
#include "xbEngine.h"

#include <SDL.h>
#include <cassert> // for assert macro
#include <cstdio> // for printf
#include <chrono>
#include <thread> // for multithreading and sleeping the thread

//NOTE[ALEX]: platform dependent code should stay in this file,
//            all other files should be independent of the platform

struct PlatformWindow {
    SDL_Window   *window;
    SDL_Surface  *surface;
    SDL_Renderer *renderer;
};

PlatformWindow *platformOpenWindow(char *windowTitle,
                        uint32_t createWidth, uint32_t createHeight       )
{
    PlatformWindow *platformWindow = (PlatformWindow *)malloc(sizeof(PlatformWindow));
    platformWindow->window   = 0;
    platformWindow->surface  = 0;
    platformWindow->renderer = 0;

    int sdlInitCode = SDL_Init(SDL_INIT_VIDEO);
    if (sdlInitCode == 0) {
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
    } else {
        printf("%s SDL_Init failed with code %i\n", __FUNCTION__, sdlInitCode);
    }
    //TODO[ALEX]: surface
    
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
    //TODO[ALEX]: surface
    
    SDL_Quit();

    if (platformWindow) {
        free(platformWindow);
    } else {
        printf("%s no memory to free\n", __FUNCTION__);
    }
}

void platformGetWindowSize(PlatformWindow *platformWindow, int *width, int *height)
{
    SDL_GetWindowSize(platformWindow->window, width, height);
}

struct PlatformSoundDevice {
    int tmp;
};

PlatformSoundDevice *platformOpenSoundDevice()
{
    PlatformSoundDevice *platformSoundDevice = (PlatformSoundDevice *)malloc(sizeof(PlatformSoundDevice));
    return platformSoundDevice;
}

void platformCloseSoundDevice(PlatformSoundDevice *platformSoundDevice)
{
    if (platformSoundDevice) {
        free(platformSoundDevice);
    } else {
        printf("%s no memory to free\n", __FUNCTION__);
    }
}

FileReadResultDEBUG platformReadEntireFileDEBUG(char *fileName)
{
    FileReadResultDEBUG fileReadResult = {};

    SDL_RWops *file = SDL_RWFromFile(fileName, "rb"); // open a file in binary read-only mode

    if (file != NULL) {
        uint64_t fileSize = file->size(file);
        if (fileSize > 0) {
            fileReadResult.contents = (void *)malloc(fileSize);
            if (fileReadResult.contents) {
                if (SDL_RWread(file, fileReadResult.contents, fileSize, 1)) {
                    fileReadResult.contentSize = fileSize;
                    printf("%s read successful, bytes read: %lu\n", __FUNCTION__, fileSize);
                } else {
                    platformFreeFileMemoryDEBUG(fileReadResult.contents);
                    fileReadResult.contentSize = 0;
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

int32_t platformWriteEntireFileDEBUG(char *fileName, void *memory, uint64_t memorySize)
{
    int32_t result = 0;

    SDL_RWops *file = SDL_RWFromFile(fileName, "wb"); // (over)write a file in binary write-only mode
    
    if (file != NULL) {
        if (SDL_RWwrite(file, memory, memorySize, 1)) { // returns number of objects written (here 1)
            uint64_t fileSize = file->size(file);
            result = (int32_t)(fileSize == memorySize);
            printf("%s write successful, bytes given: %lu, bytes written: %lu\n",
                         __FUNCTION__, memorySize, fileSize                      );
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
    assert(buttonState->isDown == 1); // there is no keyrepeat for letting go of a key
    buttonState->isDown = 0;
    buttonState->transitionCount += 1;
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

void SDL2ControllerButtonDown(SDL_Event cButtonEvent, ControllerInput *controllerInput)
{
    uint32_t controllerID = cButtonEvent.cbutton.which; //TODO[ALEX]: test this
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

void SDL2ControllerButtonUp(SDL_Event cButtonEvent, ControllerInput *controllerInput)
{
    uint32_t controllerID = cButtonEvent.cbutton.which; //TODO[ALEX]: test this
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

struct PlatformTexture {
    SDL_Texture *textureHandle;
};

void platformResizeTexture(GameBuffer *gameBuffer, GameTexture *gameTexture)
//SDL_Renderer *renderer, sdlTexture *texture)
{

    if (gameTexture->memory->textureHandle != 0) {
        SDL_DestroyTexture(gameTexture->memory->textureHandle); 
    }
    if (gameTexture->textureMemory) { 
        free(gameTexture->textureMemory); 
    }

    if (gameBuffer->memory->renderer) {
        gameTexture->memory->textureHandle = SDL_CreateTexture(gameBuffer->memory->renderer,
                                                               SDL_PIXELFORMAT_ARGB8888,
                                                               SDL_TEXTUREACCESS_STREAMING,
                                                               gameTexture->width,
                                                               gameTexture->height          );
        gameTexture->textureMemory = malloc(  gameTexture->bytesPerPixel
                                            * gameTexture->width * gameTexture->height);
    } else {
        printf("%s renderer not initialized\n", __FUNCTION__);
    }
}

void platformHandleEvents(GameBuffer *gameBuffer, GameInput *gameInput, GameGlobal *gameGlobal)
{
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
                        platformGetWindowSize(gameBuffer->memory,
                                              &gameGlobal->testTexture.width,
                                              &gameGlobal->testTexture.height);
                        platformResizeTexture(gameBuffer, &gameGlobal->testTexture);
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
            } break;
            case SDL_MOUSEBUTTONUP: {
            } break;
            case SDL_MOUSEWHEEL: {
            } break;
            case SDL_MOUSEMOTION: { // position of mouse pixel pos in window
                //TODO[ALEX]: does this stick?
                gameInput->mousePosX = event.motion.x;
                gameInput->mousePosY = event.motion.y;
            } break;
            case SDL_CONTROLLERDEVICEADDED: {
            } break;
            case SDL_CONTROLLERDEVICEREMOVED: {
            } break;
            case SDL_CONTROLLERDEVICEREMAPPED: {
            } break;
            case SDL_CONTROLLERBUTTONDOWN: {
                //TODO[ALEX]: multiple controllers
                SDL2ControllerButtonDown(event, gameInput->controller);
            } break;
            case SDL_CONTROLLERBUTTONUP: {
                SDL2ControllerButtonUp(event, gameInput->controller);
            } break;
            case SDL_CONTROLLERAXISMOTION: {
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

void platformUpdateWindow(GameBuffer *gameBuffer, GameTexture *gameTexture) {
    SDL_UpdateTexture(gameTexture->memory->textureHandle, 0, gameTexture->textureMemory,
                      gameTexture->width * gameTexture->bytesPerPixel                   );
    SDL_RenderCopy(gameBuffer->memory->renderer, gameTexture->memory->textureHandle, 0, 0);
    SDL_RenderPresent(gameBuffer->memory->renderer);
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
        for (uint64_t i = 0; i < gameMemory.permanentMemSize/sizeof(uint64_t); i++) {
            gameMemory.permanentMem[i] = 0;
        }
        for (uint64_t i = 0; i < gameMemory.transientMemSize/sizeof(uint64_t); i++) {
            gameMemory.transientMem[i] = 0;
        }
        gameMemory.initialized =  1;
    } else {
        printf("Could not allocated gameMemory: transient: %lu, permanent: %lu\n",
               (uint64_t)gameMemory.transientMem, (uint64_t)gameMemory.permanentMem);
        return 0;
    }

    assert(gameMemory.initialized);

    printf("size of gameMemory.permanentMemSize: %lu\n", gameMemory.permanentMemSize);
    printf("size of gameMemory.transientMemSize: %lu\n", gameMemory.transientMemSize);
    printf("size of gameGlobal: %lu\n", sizeof(GameGlobal));
    printf("size of gameState: %lu\n", sizeof(GameState));
    printf("size of gameInput: %lu\n", sizeof(GameInput));
    printf("size of gameClocks: %lu\n", sizeof(GameClocks));
    printf("size of gameBuffer: %lu\n", sizeof(GameBuffer));
    printf("size of gameSound: %lu\n", sizeof(GameSound));

    assert(sizeof(GameState) <= gameMemory.permanentMemSize);

    GameState  *gameState  = (GameState *)gameMemory.permanentMem;
    GameGlobal *gameGlobal = &gameState->gameGlobal;
    GameInput  *gameInput  = &gameState->gameInput;
    GameClocks *gameClocks = &gameState->gameClocks;
    GameBuffer *gameBuffer = &gameState->gameBuffer;
    GameSound  *gameSound  = &gameState->gameSound;

    assert(   &gameInput->terminator - &gameInput->keys[0]
           == sizeof(gameInput->keys)/sizeof(gameInput->keys[0]));

    gameClocks->timeStart   = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    gameClocks->cyclesStart = __rdtsc();

    gameBuffer->memory = platformOpenWindow((char *)WINDOW_TITLE,
                                            WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT    );

    gameSound->memory = platformOpenSoundDevice();

    //NOTE[ALEX]: test texture
    gameGlobal->testTexture.width         = 1024;
    gameGlobal->testTexture.height        = 1024;
    gameGlobal->testTexture.bytesPerPixel = 4;
    gameGlobal->testTexture.memory        = (PlatformTexture *)malloc(sizeof(PlatformTexture));
    gameGlobal->testTexture.memory->textureHandle = 0;
    platformGetWindowSize(gameBuffer->memory, &gameGlobal->testTexture.width,
                                              &gameGlobal->testTexture.height);
    platformResizeTexture(gameBuffer, &gameGlobal->testTexture);

    // MAIN LOOP
    while (!gameGlobal->quitGame) {
        platformHandleEvents(gameBuffer, gameInput, gameGlobal);

        if (gameGlobal->stopRendering) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MINIMIZED_SLEEP_TIME));
            continue;
        }

        gameUpdate(gameState, &gameMemory);

        platformUpdateWindow(gameBuffer, &gameGlobal->testTexture);
    }

    // CLEANUP
    platformCloseWindow((PlatformWindow *)gameBuffer->memory);
    platformCloseSoundDevice((PlatformSoundDevice *)gameSound->memory);

    //TODO[ALEX]: testTexture is the global backbuffer!
    if (gameGlobal->testTexture.textureMemory) {
        free(gameGlobal->testTexture.textureMemory); 
    } else {
        printf("gameGlobal->testTexture.textureMemory nothing to free\n");
    }
    if (gameGlobal->testTexture.memory) {
        free(gameGlobal->testTexture.memory); 
    } else {
        printf("gameGlobal->testTexture.memory nothing to free\n");
    }

    free(gameMemory.transientMem);
    free(gameMemory.permanentMem);

    return 0;
}
