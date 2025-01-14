#include "xbEngine.h"

#include <cstdio> // for printf

// get the position (id) of a specific key in GameInput->keys
uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput) {
    //TODO[ALEX]: error checking?
    uint32_t id = buttonState - &gameInput->keys[0];
    return id;
}

void gameUpdate(GameState *gameState, GameMemory *gameMemory)
{
    uint32_t keyCount = sizeof(gameState->gameInput.keys)/sizeof(gameState->gameInput.keys[0]);
    for (uint32_t i = 0; i < keyCount; i++) {
        if (gameState->gameInput.keys[i].isDown) {
            printf("Key %u held down\n", i);
        }
        if (gameState->gameInput.keys[i].transitionCount > 1) {
            uint32_t presses = gameState->gameInput.keys[i].transitionCount/2;
            printf("Key %u pressed %u times\n", i, presses);
            gameState->gameInput.keys[i].transitionCount %= 2;
        }
    }

    // texture test
    if (gameState->gameInput.s.isDown) {
        gameState->gameGlobal.offsetX -= 1;
    }
    if (gameState->gameInput.f.isDown) {
        gameState->gameGlobal.offsetX += 1;
    }
    if (gameState->gameInput.d.isDown) {
        gameState->gameGlobal.offsetY -= 1;
    }
    if (gameState->gameInput.e.isDown) {
        gameState->gameGlobal.offsetY += 1;
    }
    
    uint32_t pitch =  gameState->gameGlobal.testTexture.width
                    * gameState->gameGlobal.testTexture.bytesPerPixel;
    uint8_t  *row  = (uint8_t *)gameState->gameGlobal.testTexture.textureMemory;
    for (int y = 0; y < gameState->gameGlobal.testTexture.height; y++) {
        uint8_t *pixel = (uint8_t *)row;
        for (int x = 0; x < gameState->gameGlobal.testTexture.width; x++) {
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

    // // retrieve id of specific key:
    // printf("ID of key 'x': %u\n", getKeyID(&gameState->gameInput.x, &gameState->gameInput));

    // // waste some cycles to test key press count
    // int x = 8;
    // for (int i = 0; i < 1000*1000*1000; i++) {
    //     x *= x;
    // }

    gameState->gameGlobal.gameFrame++;
    // printf("%s frame %lu complete\n", __FUNCTION__, gameState->gameGlobal.gameFrame);
}
