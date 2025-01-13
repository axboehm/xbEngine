#include "constants.h"
#include "xbEngine.h"

#include <SDL.h>

int32_t renderTest = 0;
int32_t running = 1;

void handleEvent(SDL_Event *event)
{
    switch (event->type) {
        case SDL_QUIT: {
            printf("SDL_QUIT\n");
            running = 0;
            break;
        }
        case SDL_WINDOWEVENT: {
            switch (event->window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n",
                           event->window.data1, event->window.data2);
                    break;
                }
                case SDL_WINDOWEVENT_EXPOSED: {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);

                    if (renderTest) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        renderTest = 0;
                    } else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        renderTest = 1;
                    }

                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;
                }
            }
            break;
        }
    }
}

int main(int argc, char **argv)
{
    // INITIALIZATION
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error!", "Failed to initialize SDL.", 0);
    }

    SDL_Window *window;
    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              1920, 1080, SDL_WINDOW_RESIZABLE);
    if (window) {
        SDL_Renderer *sdlRenderer = SDL_CreateRenderer(window, -1, 0);

        if (sdlRenderer) {
            // MAIN LOOP
            while (running) {
                SDL_Event event;
                SDL_PollEvent(&event);

                handleEvent(&event);
            }

            // CLEANUP
            SDL_Quit();
        } else {
            printf("SDL_CreateRenderer failed.\n");
        }
    } else {
        printf("SDL_CreateWindow failed.\n");
    }

    return 0;
}
