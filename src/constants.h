#ifndef CONSTANTS_H // include guard begin
#define CONSTANTS_H // include guard

// MATH
#define Kilobytes(value) (((uint64_t)value) * 1024LL)
#define Megabytes(value) (((uint64_t)value) * 1024LL * 1024LL)
#define Gigabytes(value) (((uint64_t)value) * 1024LL * 1024LL * 1024LL)
#define PI32 3.14159265358979f

// ASSERTIONS (XB_SLOW to be defined when compiling
#if XB_SLOW
#define xbAssert(expression) if(!(expression)) { *(int *)0 = 0; }
#else
#define xbAssert(expression)
#endif

// DEBUG
// #define INPUT_TEST
#define INPUT_TEST_MOUSE
#define INPUT_TEST_AXES
#define INPUT_TEST_PRESSES
#define INPUT_TEST_DOWNS
// #define SQUARE_WAVE_TEST

// WINDOW
#define WINDOW_TITLE "xbEngine_Window_Title"
#define WINDOW_INIT_WIDTH 1920
#define WINDOW_INIT_HEIGHT 1080
// CONTROLLERS
#define MAX_CONTROLLERS 4
//NOTE[ALEX]: deadzones are hardcoded for now, this should become part of the settings later
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

// ENGINE CONSTANTS
#define MINIMIZED_SLEEP_TIME 100 // in ms

#endif // include guard end
