# Overview

This project is a platform layer for cross platform applications or games. <br>
The code interfaces with the OS using the [SDL2 library](https://www.libsdl.org/) as it's only dependency. <br>
This is not a final platform layer but a work in progress. I hope it can be useful as a reference for anyone trying to create an engine themselves. <br>

![xbEngine](./xbEngineImg01.png)


# Features and Usage
Running the application opens a window with a scrollable background, an audio output (sine wave) with variable frequency controlled by the vertical mouse position and a visualization for keyboard input. <br>
The application handles audio, input (keyboard, mouse and controller) and the application window using SDL2. Keeping track of the timestep is part of the platform layer, allowing for the application to be framerate independent. <br>
All platform dependent code is in the `sdl_xbEngine.cpp` file. This file also contains the entrypoint into the code. It calls into the application/game code, which is platform independent. <br>
Memory is managed entirely in the platform dependent part and passed by pointer. <br>
<br>
There is a good amount of debugging output when running the application from the terminal. More can be enabled or disabled by setting flags in the `constants.h` file. <br>
To extend the application, call into your own code from the main loop in `sdl_xbEngine.cpp` or extend my code in `xbEngine.cpp`. <br>


# How to Build
The project calls into SDL2 and requires a library to be available on the user's system. It can be compiled manually or the distribution's package can be used. I used version 2.30.0 but any version after 2.0.4 should work. <br>
[SDL2 wiki](https://wiki.libsdl.org/SDL2/Installation) <br>
or on ubuntu based systems: <br>
`sudo apt install libsdl2-dev` <br>
<br>
In either case, adjust the relevant section of the Makefile to link against the library on your system. <br>
<br>
C++ build tools can be installed on ubuntu based systems with: <br>
`sudo apt install build-essential` <br>
<br>
I have used g++ on Linux Mint 21 using a Makefile to build. <br>
To build (adjust the Makefile!): <br>
`cd src` <br>
`make` <br>
To run: <br>
`build/pman` <br>
