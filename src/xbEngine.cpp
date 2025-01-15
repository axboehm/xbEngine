#include "xbEngine.h"
#include "constants.h"

#include <cstdio> // for printf

// get the position (id) of a specific key in GameInput->keys
uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput) {
    //TODO[ALEX]: error checking?
    uint32_t id = buttonState - &gameInput->keys[0];
    return id;
}

void gameUpdate(GameState *gameState, GameMemory *gameMemory)
{
    if (gameState->gameInput.esc.isDown) {
        gameState->gameGlobal.quitGame = true;
        return;
    }

#ifdef INPUT_TEST
    // mouse test
#ifdef INPUT_TEST_MOUSE
    printf("Mouse pos(x, y): %u, %u, scr(h, v): %i, %i\n",
           gameState->gameInput.mousePosX, gameState->gameInput.mousePosY,
           gameState->gameInput.mouseScrH, gameState->gameInput.mouseScrV );
#endif

    uint32_t mButtonCount =  sizeof(gameState->gameInput.mButtons)
                            /sizeof(gameState->gameInput.mButtons[0]);
    for (uint32_t i = 0; i < mButtonCount; i++) {
#ifdef INPUT_TEST_DOWNS
        if (gameState->gameInput.mButtons[i].isDown) {
            printf("Mouse button %u held down\n", i);
        }
#endif
#ifdef INPUT_TEST_PRESSES
        if (gameState->gameInput.mButtons[i].transitionCount > 1) {
            uint32_t presses = gameState->gameInput.mButtons[i].transitionCount/2;
            printf("Mouse button %u pressed %u times\n", i, presses);
            gameState->gameInput.mButtons[i].transitionCount %= 2;
        }
#endif
    }

    // keyboard test
    uint32_t keyCount = sizeof(gameState->gameInput.keys)/sizeof(gameState->gameInput.keys[0]);
    for (uint32_t i = 0; i < keyCount; i++) {
#ifdef INPUT_TEST_DOWNS
        if (gameState->gameInput.keys[i].isDown) {
            printf("Key %u held down\n", i);
        }
#endif
#ifdef INPUT_TEST_PRESSES
        if (gameState->gameInput.keys[i].transitionCount > 1) {
            uint32_t presses = gameState->gameInput.keys[i].transitionCount/2;
            printf("Key %u pressed %u times\n", i, presses);
            gameState->gameInput.keys[i].transitionCount %= 2;
        }
#endif
    }

    // controller test
    uint32_t buttonCount =  sizeof(gameState->gameInput.controller[0].buttons)
                           /sizeof(gameState->gameInput.controller[0].buttons[0]);
    uint32_t axesCount   =  sizeof(gameState->gameInput.controller[0].axes)
                           /sizeof(gameState->gameInput.controller[0].axes[0]);
    for (uint32_t i = 0; i < MAX_CONTROLLERS; i++) {
        for (uint32_t j = 0; j < buttonCount; j++) {
#ifdef INPUT_TEST_DOWNS
            if (gameState->gameInput.controller[i].buttons[j].isDown) {
                printf("Controller %u Button %u held down\n", i, j);
            }
#endif
#ifdef INPUT_TEST_PRESSES
            if (gameState->gameInput.controller[i].buttons[j].transitionCount > 1) {
                uint32_t presses = gameState->gameInput.controller[i].buttons[j].transitionCount/2;
                printf("Controller %u Button %u pressed %u times\n", i, j, presses);
                gameState->gameInput.controller[i].buttons[j].transitionCount %= 2;
            }
#endif
        }
        for (uint32_t j = 0; j < axesCount; j++) {
#ifdef INPUT_TEST_AXES
            if (gameState->gameInput.controller[i].axes[j] != 0) {
                int16_t value = gameState->gameInput.controller[i].axes[j];
                printf("Controller %u Axis %u Value: %i\n", i, j, value);
            }
#endif
        }
    }
#endif

    // texture test
    int32_t scrollSpeed = 8;
    gameState->gameGlobal.offsetX += 
        (int16_t)((float)scrollSpeed *  (float)gameState->gameInput.controller[0].leftStickX
                                      / (float)CONTR_AXIS_NORMALIZATION                     );
    gameState->gameGlobal.offsetY +=
        (int16_t)((float)scrollSpeed *  (float)gameState->gameInput.controller[0].leftStickY
                                      / (float)CONTR_AXIS_NORMALIZATION                     );
    if (gameState->gameInput.s.isDown) {
        gameState->gameGlobal.offsetX -= scrollSpeed;
    }
    if (gameState->gameInput.f.isDown) {
        gameState->gameGlobal.offsetX += scrollSpeed;
    }
    if (gameState->gameInput.d.isDown) {
        gameState->gameGlobal.offsetY -= scrollSpeed;
    }
    if (gameState->gameInput.e.isDown) {
        gameState->gameGlobal.offsetY += scrollSpeed;
    }
    
    uint32_t pitch =  gameState->gameBuffer.backBuffer.width
                    * gameState->gameBuffer.backBuffer.bytesPerPixel;
    uint8_t  *row  = (uint8_t *)gameState->gameBuffer.backBuffer.textureMemory;
    for (int y = 0; y < gameState->gameBuffer.backBuffer.height; y++) {
        uint8_t *pixel = (uint8_t *)row;
        for (int x = 0; x < gameState->gameBuffer.backBuffer.width; x++) {
            *pixel = (uint8_t)(x+gameState->gameGlobal.offsetX);
            pixel++;
            *pixel = (uint8_t)(y+gameState->gameGlobal.offsetY);
            pixel++;
            *pixel = 0;
            pixel++;
            *pixel = 255;
            pixel++;
        }
        row += pitch;
    }

    // audio test

    // retrieve id of specific key:
    // printf("ID of key 'x': %u\n", getKeyID(&gameState->gameInput.x, &gameState->gameInput));

    // waste some cycles to test key press count
    // int x = 8;
    // for (int i = 0; i < 1000*1000*1000; i++) {
    //     x *= x;
    // }

    gameState->gameGlobal.gameFrame++;
}
