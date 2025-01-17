#ifndef XBENGINE_H // include guard begin
#define XBENGINE_H // include guard

#include "constants.h"

#include <stdint.h> // defines fixed size types, C++ version is <cstdint>

struct GameMemory {
    uint8_t   initialized;
    uint64_t  permanentMemSize;
    uint64_t *permanentMem; //NOTE[ALEX]: this is uint64_t, only so that I can initialize to 0
    uint64_t  transientMemSize;
    uint64_t *transientMem;
};

struct GameTexture {
    int       width;
    int       height;
    uint32_t  bytesPerPixel;
    void     *memory; // PlatformTexture
    void     *textureMemory; //NOTE[ALEX]: currently the texture memory is not
                             //            part of the allocated memory but separate from it
};

struct GameGlobal {
    int32_t  quitGame;
    int32_t  stopRendering;
    uint32_t monitorRefreshRate;   // refresh rate that the monitor supports (0 if unknown)
    uint32_t renderingRefreshRate; // refresh rate used to render (can be set manually)
    float    targetTimePerFrame;   // in ms
    uint64_t gameFrame; // counts frames

    //TODO[ALEX]: testing!
    // gradient
    int32_t  offsetX; //TODO[ALEX]: this should be in transient memory!
    int32_t  offsetY;
    // audio
    float    tWave;
    uint32_t runningSampleIndex;
    uint32_t toneHz;
    uint32_t toneVolume;
    uint32_t wavePeriod;
    uint32_t halfWavePeriod;
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
    void            *platformControllers[MAX_CONTROLLERS]; // PlatformController
    uint8_t          controllerConnected[MAX_CONTROLLERS];
    ControllerInput  controller[MAX_CONTROLLERS];

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
    uint64_t perfCountFrequency;

    uint64_t lastPerfCounter;
    uint64_t endPerfCounter;
    uint64_t elapsedPerfCounter;

    float msLastFrame;
    float msLastFrameCPU; // without main loop waiting

    uint64_t lastCycleCount;
    uint64_t endCycleCount;
    uint64_t elapsedCycleCount;
};

struct GameBuffer {
    void        *memory; // PlatformWindow
    GameTexture  backBuffer;
    uint32_t     width;
    uint32_t     height;
};

struct GameSound {
    uint16_t bytesPerSamplePerChannel;
    int16_t  audioToQueue[AUDIO_MAX_LATENCY_SECONDS*AUDIO_SAMPLES_PER_SECOND*AUDIO_CHANNELS];
    uint32_t audioToQueueBytes; // how many new bytes to queue up
    uint32_t queuedBytes;       // how many bytes are currently queued up
    uint32_t targetQueuedBytes; // controls latency (how many bytes to queue up at most)
};

struct GameState {
    GameGlobal gameGlobal;
    GameInput  gameInput; //TODO[ALEX]: multiple of these?
                          // - how to make my life easier for potential multiplayer
    GameClocks gameClocks;
    GameBuffer gameBuffer;
    GameSound  gameSound;
};

void gameUpdate(GameState *gameState, GameMemory *gameMemory);

uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput);

#endif // include guard end
