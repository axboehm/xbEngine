#ifndef XBENGINE_H // include guard begin
#define XBENGINE_H // include guard

#include "constants.h"

#include <stdint.h> // defines fixed size types, C++ version is <cstdint>

#define Kilobytes(value) (((uint64_t)value) * 1024)
#define Megabytes(value) (((uint64_t)value) * 1024 * 1024)
#define Gigabytes(value) (((uint64_t)value) * 1024 * 1024 * 1024)

struct PlatformWindow;

PlatformWindow *platformOpenWindow(char *windowTitle,
                                   uint32_t createWidth, uint32_t createHeight);
void platformCloseWindow(PlatformWindow *platformWindow);
void platformGetWindowSize(PlatformWindow *platformWindow, int *width, int *height);

struct PlatformSoundDevice;

PlatformSoundDevice *platformOpenSoundDevice();
void platformCloseSoundDevice(PlatformSoundDevice *platformSoundDevice);

struct PlatformTexture;

struct FileReadResultDEBUG {
    uint32_t  contentSize;
    void     *contents;
};

struct PlatformController;

FileReadResultDEBUG platformReadEntireFileDEBUG(char *fileName);
void platformFreeFileMemoryDEBUG(void *memory);
int32_t platformWriteEntireFileDEBUG(char *fileName, void *memory, uint64_t memorySize);

struct GameMemory {
    int32_t   initialized;
    uint64_t  permanentMemSize;
    uint64_t *permanentMem; //NOTE[ALEX]: this is uint64_t, only so that I can initialize to 0
    uint64_t  transientMemSize;
    uint64_t *transientMem;
};

struct GameTexture {
    int              width;
    int              height;
    uint32_t         bytesPerPixel;
    PlatformTexture *memory;
    void            *textureMemory; //NOTE[ALEX]: currently the texture memory is not
                                    //            part of the allocated memory but separate from it
};

struct GameGlobal {
    int32_t  quitGame;
    int32_t  stopRendering;
    uint64_t gameFrame;

    int32_t offsetX; // for testing
    int32_t offsetY;
};

struct ButtonState {
    int8_t  isDown;
    uint8_t transitionCount; // pressing or letting go of a button counts as a transition
};

struct ControllerInput {
    union {
        int16_t axes[6];
        struct {
            int16_t leftStickX;
            int16_t leftStickY;
            int16_t rightStickX;
            int16_t rightStickY;
            int16_t leftTrigger;
            int16_t rightTrigger;
        };
    };
    union {
        ButtonState buttons[17];
        struct {
            ButtonState fUp;
            ButtonState fDown;
            ButtonState fLeft;
            ButtonState fRight;

            ButtonState dUp;
            ButtonState dDown;
            ButtonState dLeft;
            ButtonState dRight;

            ButtonState shoulderLeft;
            ButtonState shoulderRight;

            ButtonState stickClickLeft;
            ButtonState stickClickRight;

            ButtonState start;
            ButtonState guide;
            ButtonState select;

            ButtonState misc;   // xbox share button, ps microphone button
            ButtonState touch;  // ps touchpad press

            ButtonState terminatorContr; // always the last button
        };
    };
};

struct GameInput {
    PlatformController *platformControllers[MAX_CONTROLLERS];
    ControllerInput     controller[MAX_CONTROLLERS];

    uint32_t mousePosX;
    uint32_t mousePosY;
    int32_t  mouseScrH;
    int32_t  mouseScrV;
    union {
        ButtonState mButtons[5];
        struct {
            ButtonState m1;
            ButtonState m2;
            ButtonState m3;
            ButtonState m4;
            ButtonState m5;

            ButtonState terminatorMouse; // always the last button
        };
    };
    union {
        ButtonState keys[101];
        struct {
            ButtonState a;
            ButtonState b;
            ButtonState c;
            ButtonState d;
            ButtonState e;
            ButtonState f;
            ButtonState g;
            ButtonState h;
            ButtonState i;
            ButtonState j;
            ButtonState k;
            ButtonState l;
            ButtonState m;
            ButtonState n;
            ButtonState o;
            ButtonState p;
            ButtonState q;
            ButtonState r;
            ButtonState s;
            ButtonState t;
            ButtonState u;
            ButtonState v;
            ButtonState w;
            ButtonState x;
            ButtonState y;
            ButtonState z;

            ButtonState backQuote;
            ButtonState minus;
            ButtonState equals;
            ButtonState leftBracket;
            ButtonState rightBracket;
            ButtonState semicolon;
            ButtonState quote;
            ButtonState comma;
            ButtonState period;
            ButtonState slash;

            ButtonState one;
            ButtonState two;
            ButtonState three;
            ButtonState four;
            ButtonState five;
            ButtonState six;
            ButtonState seven;
            ButtonState eight;
            ButtonState nine;
            ButtonState zero;

            ButtonState esc;
            ButtonState tab;
            ButtonState caps;
            ButtonState backspace;
            ButtonState backslash;
            ButtonState enter;

            ButtonState space;

            ButtonState lCtrl;
            ButtonState rCtrl;
            ButtonState lShift;
            ButtonState rShift;
            ButtonState lAlt;
            ButtonState rAlt;

            ButtonState insert;
            ButtonState home;
            ButtonState pageUp;
            ButtonState del;
            ButtonState end;
            ButtonState pageDown;

            ButtonState up;
            ButtonState down;
            ButtonState left;
            ButtonState right;
            
            ButtonState f1;
            ButtonState f2;
            ButtonState f3;
            ButtonState f4;
            ButtonState f5;
            ButtonState f6;
            ButtonState f7;
            ButtonState f8;
            ButtonState f9;
            ButtonState f10;
            ButtonState f11;
            ButtonState f12;
            
            ButtonState printScreen;
            ButtonState scrollLock;
            ButtonState pause;

            ButtonState numLock;
            ButtonState numDivide;
            ButtonState numMultiply;
            ButtonState numMinus;
            ButtonState numPlus;
            ButtonState numEnter;
            ButtonState numPeriod;
            ButtonState numOne;
            ButtonState numTwo;
            ButtonState numThree;
            ButtonState numFour;
            ButtonState numFive;
            ButtonState numSix;
            ButtonState numSeven;
            ButtonState numEight;
            ButtonState numNine;
            ButtonState numZero;

            ButtonState terminatorKeys; // always the last button
        };
    };
};

struct GameClocks {
    int64_t  timeStart;
    int64_t  timeEnd;
    int64_t  timeLastFrame;
    uint64_t cyclesStart;
    uint64_t cyclesEnd;
    uint64_t cyclesLastFrame;
};

struct GameBuffer {
    PlatformWindow *memory;
    GameTexture     backBuffer;
    uint32_t        width;
    uint32_t        height;
};

struct GameSound {
    PlatformSoundDevice *memory;
};

struct GameState {
    GameGlobal gameGlobal;
    GameInput  gameInput; //TODO[ALEX]: multiple of these?
                          // - how to make my life easier for potential multiplayer
    GameClocks gameClocks;
    GameBuffer gameBuffer;
    GameSound  gameSound;
};

void platformHandleEvents(GameInput *gameInput, GameGlobal *gameGlobal);
void gameUpdate(GameState *gameState, GameMemory *gameMemory);

uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput);

void platformCloseBuffer(GameBuffer *gameBuffer);
void platformResizeTexture(GameBuffer *gameBuffer, GameTexture *gameTexture);
void platformInitializeControllers(GameInput *gameInput);
void platformResetControllers(GameInput *gameInput);
void platformCloseControllers(GameInput *gameInput);

#endif // include guard end
