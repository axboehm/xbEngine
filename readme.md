# xbEngine

This project is a platform layer for cross platform applications or games. <br>
The code interfaces with the OS using the SDL2 library as it's only dependency. <br>

![xbEngine](./xbEngineImg01.png)


# Features and Usage
The application handles audio, keyboard, mouse and controller input and the application window using SDL2. Keeping track of the timestep is part of the platform layer, allowing for the application to be framerate independent. <br>
All platform dependent code is in the ```sdl_xbEngine.cpp```cpp file. This file also contains the entrypoint into the code. It calls into the application/game code, which is platform independent. <br>
Memory is managed entirely in the platform dependent part. <br>
<br>
Running the application opens a window with a scrollable background, an audio wave (sine wave) with variable frequency controlled by the vertical mouse position and a visualization for keyboard input. <br>
There is a good amount of debugging output when running the application from the terminal. More can be enabled or disabled by setting flags in the `constants.h` file. <br>
To extend the application, call into your own code from the main loop in `sdl_xbEngine.cpp` or extend my code in `xbEngine.cpp`. <br>


# Building

The project was created on Linux and uses a Makefile for building. <br>
The included Makefile links against a compiled SDL2 static library. Adjust the Makefile accordingly to use with your own or your systems libraries. <br>
