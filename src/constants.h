#ifndef CONSTANTS_H // include guard begin
#define CONSTANTS_H // include guard

// DEBUG
#define INPUT_TEST
#define INPUT_TEST_MOUSE
#define INPUT_TEST_AXES
#define INPUT_TEST_PRESSES
#define INPUT_TEST_DOWNS

// GENERAL
#define WINDOW_TITLE "xbEngine_Window_Title"
#define WINDOW_INIT_WIDTH 1920
#define WINDOW_INIT_HEIGHT 1080
#define MAX_CONTROLLERS 4
//NOTE[ALEX]: deadzones are hardcoded for now, this should become part of the settings later
// max 65536/2 (int16)
#define CONTR_AXIS_DEADZONE_INNER 2000
#define CONTR_AXIS_DEADZONE_OUTER 30000
#define CONTR_AXIS_NORMALIZATION 32767

// ENGINE CONSTANTS
#define MINIMIZED_SLEEP_TIME 100 // in ms

#endif // include guard end
