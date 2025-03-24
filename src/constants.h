#ifndef CONSTANTS_H // include guard begin
#define CONSTANTS_H // include guard

#include <cstdio> // for printf in assert macro

// MATH
#define Kilobytes(value) (((uint64_t)value) * 1024LL)
#define Megabytes(value) (((uint64_t)value) * 1024LL * 1024LL)
#define Gigabytes(value) (((uint64_t)value) * 1024LL * 1024LL * 1024LL)
#define PI32 3.14159265358979f

// ASSERTIONS (XB_SLOW to be defined when compiling)
//NOTE[ALEX]: this custom assertion intentionally crashes the executable
//            it could be replaced with the standard library's assert() (requires #include <cassert>)
//            newer versions of gcc or clang do not crash the program when deferencing a
//            null pointer anymore, so if `*(int *)0 = 0` does not work, try `__builtin_trap()`
//            this custom assertion crashes slower than the standard c assert but does not
//            require the standard library's import and is only defined when specified
#if XB_SLOW
#define xbAssert(expression) if(!(expression)) { \
    printf("xbAssert! %s, %s:%d - assertion '%s' failed.\n", \
           __FILE__, __FUNCTION__, __LINE__, #expression); \
    __builtin_trap(); \
}
#else
#define xbAssert(expression)
#endif

// DEBUG
#define PRINT_MEMORY_SIZES
// #define PRINT_FRAME_TIMES
// #define INPUT_TEST_MOUSE
// #define INPUT_TEST_AXES
// #define INPUT_TEST_PRESSES
#define INPUT_TEST_DOWNS
#define MULTI_THREADING_TEST

// WINDOW & GAMEBUFFER
#define WINDOW_TITLE "xbEngine_Window_Title"
#define WINDOW_INIT_WIDTH 1920
#define WINDOW_INIT_HEIGHT 1080
#define WINDOW_MAX_WIDTH 3840
#define WINDOW_MAX_HEIGHT 2160
#define GAMEBUFFER_BYTES_PER_PIXEL 4

// CONTROLLERS
#define MAX_CONTROLLERS 4
//NOTE[ALEX]: deadzones are hardcoded for now, this should become part of the settings later
//            players should also be able to calibrate this manually
// max 65536/2 (int16)
#define CONTR_AXIS_DEADZONE_INNER 2000
#define CONTR_AXIS_DEADZONE_OUTER 30000
#define CONTR_AXIS_NORMALIZATION 32767

// AUDIO
#define AUDIO_SAMPLES_PER_SECOND 48000
#define AUDIO_CHANNELS 2
// 48000/fps -> 800 for 60fps, 1 frame latency
#define AUDIO_SAMPLES_PER_CALL 800
#define AUDIO_MAX_LATENCY_SECONDS 2
// game targets at least 30fps, so audio should target the minimum to avoid missing target
#define AUDIO_REFRESH_RATE 30

// ENGINE CONSTANTS
#define MINIMIZED_WAIT_TIME 100
#define THREAD_COUNT 3 // excluding main thread
#define THREAD_NAME "xbThread"
#define WORK_QUEUE_ENTRIES 256

#endif // include guard end
