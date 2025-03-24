#ifndef XBENGINE_H // include guard begin
#define XBENGINE_H // include guard

#include "constants.h"

#include <stdint.h> // defines fixed size types, C++ version is <cstdint>

struct GameMemory {
    uint8_t   initialized;
    uint64_t  permanentMemSize;
    void     *permanentMem;
    uint64_t  transientMemSize;
    void     *transientMem;
};

// PERMANENT MEMORY
struct GameGlobal {
    int32_t  quitGame;
    int32_t  stopRendering;
    uint32_t monitorRefreshRate;   // refresh rate that the monitor supports (0 if unknown)
    uint32_t renderingRefreshRate; // refresh rate used to render (can be set manually)
    float    targetTimePerFrame;   // in ms
    uint64_t gameFrame;            // counts total frames since startup
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
    void            *platformController[MAX_CONTROLLERS];
    uint8_t          controllerConnected[MAX_CONTROLLERS];
    ControllerInput  controller[MAX_CONTROLLERS];

    int mousePosX;
    int mousePosY;
    int mouseScrH;
    int mouseScrV;
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
        //NOTE[ALEX]: order like international keyboard layout (left to right, top to bottom)
        //            only relevant for debug printing of keyboard presses or if buttons are
        //            addressed by an array index
        ButtonState keys[101];
        struct {
            // row 0
            ButtonState esc;

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

            // row 1
            ButtonState backQuote;

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
            ButtonState minus;
            ButtonState equals;

            ButtonState backspace;

            ButtonState insert;
            ButtonState home;
            ButtonState pageUp;

            ButtonState numLock;
            ButtonState numDivide;
            ButtonState numMultiply;
            ButtonState numMinus;

            // row 2
            ButtonState tab;

            ButtonState q;
            ButtonState w;
            ButtonState e;
            ButtonState r;
            ButtonState t;
            ButtonState y;
            ButtonState u;
            ButtonState i;
            ButtonState o;
            ButtonState p;

            ButtonState leftBracket;
            ButtonState rightBracket;
            ButtonState backslash;

            ButtonState del;
            ButtonState end;
            ButtonState pageDown;

            ButtonState numSeven;
            ButtonState numEight;
            ButtonState numNine;
            ButtonState numPlus;

            // row 3
            ButtonState caps;

            ButtonState a;
            ButtonState s;
            ButtonState d;
            ButtonState f;
            ButtonState g;
            ButtonState h;
            ButtonState j;
            ButtonState k;
            ButtonState l;
            ButtonState semicolon;
            ButtonState quote;

            ButtonState enter;

            ButtonState numFour;
            ButtonState numFive;
            ButtonState numSix;

            // row 4
            ButtonState lShift;

            ButtonState z;
            ButtonState x;
            ButtonState c;
            ButtonState v;
            ButtonState b;
            ButtonState n;
            ButtonState m;
            ButtonState comma;
            ButtonState period;
            ButtonState slash;

            ButtonState rShift;

            ButtonState up;

            ButtonState numOne;
            ButtonState numTwo;
            ButtonState numThree;
            ButtonState numEnter;

            // row 5
            ButtonState lCtrl;
            ButtonState lAlt;

            ButtonState space;

            ButtonState rAlt;
            ButtonState rCtrl;

            ButtonState left;
            ButtonState down;
            ButtonState right;
            
            ButtonState numZero;
            ButtonState numPeriod;

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
    void     *platformWindow;
    int       width;
    int       height;
    uint32_t  bytesPerPixel;
    uint32_t  pitch;
    void     *platformTexture;
    //NOTE[ALEX]: for an uncompressed 4K texture with 4 bytes per pixel,
    //            the allocated memory is about 32mb
    uint8_t   textureMemory[GAMEBUFFER_BYTES_PER_PIXEL*WINDOW_MAX_WIDTH*WINDOW_MAX_HEIGHT];
};

struct GameSound {
    uint16_t bytesPerSamplePerChannel;
    int16_t  audioToQueue[AUDIO_MAX_LATENCY_SECONDS*AUDIO_SAMPLES_PER_SECOND*AUDIO_CHANNELS];
    uint32_t audioToQueueBytes; // how many new bytes to queue up
    uint32_t queuedBytes;       // how many bytes are currently queued up
    uint32_t targetQueuedBytes; // controls latency (how many bytes to queue up at most)
};

struct PlatformWorkQueue; //NOTE[ALEX]: blind struct to avoid including the platform header
typedef void PlatformWorkQueueCallback(void *data, uint32_t logicalThreadID);
typedef int32_t PlatformAddWork(PlatformWorkQueue *platformQueue,
                                PlatformWorkQueueCallback *callback, void *data);
typedef void PlatformCompleteWork(PlatformWorkQueue *platformQueue, uint32_t logicalThreadID);

struct WorkQueues {
    PlatformWorkQueue *workQueue; //NOTE[ALEX]: there could be multiple of these with
                                  //            different priorities
    PlatformAddWork      *platformAddWork;
    PlatformCompleteWork *platformCompleteWork;
};

struct GameState {
    GameGlobal gameGlobal;
    GameInput  gameInput;
    GameClocks gameClocks;
    GameBuffer gameBuffer;
    GameSound  gameSound;
    WorkQueues workQueues;
};

// TRANSIENT MEMORY
//NOTE[ALEX]: for testing input, audio and rendering
struct GameTest {
    // gradient background
    int32_t  offsetX;
    int32_t  offsetY;
    // audio sine wave
    float    tWave;
    uint32_t runningSampleIndex;
    uint32_t toneHz;
    uint32_t toneVolume;
    uint32_t wavePeriod;
    uint32_t halfWavePeriod;
};

void gameUpdate(GameState *gameState, GameTest *gameTest);

void drawRectangle(float startXF, float startYF, float endXF, float endYF,
                   GameBuffer *gameBuffer, uint32_t color                 );
uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput);

#endif // include guard end
