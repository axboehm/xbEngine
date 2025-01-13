#include "xbEngine.h"

#include <SDL.h>
#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "pretest\n";
    test();

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "xbEngine", "Test", 0);

    return 0;
}
