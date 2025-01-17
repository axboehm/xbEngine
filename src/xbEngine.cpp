#include "xbEngine.h"
#include "constants.h"

#include <cstdio> // for printf
#include <math.h> // for sinf

// get the position (id) of a specific key in GameInput->keys
uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput) {
    uint32_t id = buttonState - &gameInput->keys[0];
    printf("ID of pressed key: %u\n", id);
    return id;
}

void inputTestDEBUG(GameInput *gameInput)
{
#ifdef INPUT_TEST
    // mouse test
#ifdef INPUT_TEST_MOUSE
    printf("Mouse pos(x, y): %u, %u, scr(h, v): %i, %i\n",
           gameInput->mousePosX, gameInput->mousePosY,
           gameInput->mouseScrH, gameInput->mouseScrV     );
#endif

    uint32_t mButtonCount =  sizeof(gameInput->mButtons)
                            /sizeof(gameInput->mButtons[0]);
    for (uint32_t i = 0; i < mButtonCount; i++) {
#ifdef INPUT_TEST_DOWNS
        if (gameInput->mButtons[i].isDown) {
            printf("Mouse button %u held down\n", i);
        }
#endif
#ifdef INPUT_TEST_PRESSES
        if (gameInput->mButtons[i].transitionCount > 1) {
            uint32_t presses = gameInput->mButtons[i].transitionCount/2;
            printf("Mouse button %u pressed %u times\n", i, presses);
            gameInput->mButtons[i].transitionCount %= 2;
        }
#endif
    }

    // keyboard test
    uint32_t keyCount = sizeof(gameInput->keys)/sizeof(gameInput->keys[0]);
    for (uint32_t i = 0; i < keyCount; i++) {
#ifdef INPUT_TEST_DOWNS
        if (gameInput->keys[i].isDown) {
            printf("Key %u held down\n", i);
        }
#endif
#ifdef INPUT_TEST_PRESSES
        if (gameInput->keys[i].transitionCount > 1) {
            uint32_t presses = gameInput->keys[i].transitionCount/2;
            printf("Key %u pressed %u times\n", i, presses);
            gameInput->keys[i].transitionCount %= 2;
        }
#endif
    }

    // controller test
    uint32_t buttonCount =  sizeof(gameInput->controller[0].buttons)
                           /sizeof(gameInput->controller[0].buttons[0]);
    uint32_t axesCount   =  sizeof(gameInput->controller[0].axes)
                           /sizeof(gameInput->controller[0].axes[0]);
    for (uint32_t i = 0; i < MAX_CONTROLLERS; i++) {
        for (uint32_t j = 0; j < buttonCount; j++) {
#ifdef INPUT_TEST_DOWNS
            if (gameInput->controller[i].buttons[j].isDown) {
                printf("Controller %u Button %u held down\n", i, j);
            }
#endif
#ifdef INPUT_TEST_PRESSES
            if (gameInput->controller[i].buttons[j].transitionCount > 1) {
                uint32_t presses = gameInput->controller[i].buttons[j].transitionCount/2;
                printf("Controller %u Button %u pressed %u times\n", i, j, presses);
                gameInput->controller[i].buttons[j].transitionCount %= 2;
            }
#endif
        }
        for (uint32_t j = 0; j < axesCount; j++) {
#ifdef INPUT_TEST_AXES
            if (gameInput->controller[i].axes[j] != 0) {
                int16_t value = gameInput->controller[i].axes[j];
                printf("Controller %u Axis %u Value: %i\n", i, j, value);
            }
#endif
        }
    }
#endif
}

void textureTestDEBUG(GameInput *gameInput, GameGlobal *gameGlobal,
                      GameBuffer *gameBuffer, GameClocks *gameClocks)
{
    uint32_t scrollSpeed = 8; //NOTE[ALEX]: framerate dependent
    gameGlobal->offsetX += 
        (int16_t)(scrollSpeed *   (float)gameInput->controller[0].leftStickX
                                / (float)CONTR_AXIS_NORMALIZATION           );
    gameGlobal->offsetY +=
        (int16_t)(scrollSpeed *   (float)gameInput->controller[0].leftStickY
                                / (float)CONTR_AXIS_NORMALIZATION           );
    if (gameInput->s.isDown) { gameGlobal->offsetX -= scrollSpeed; }
    if (gameInput->f.isDown) { gameGlobal->offsetX += scrollSpeed; }
    if (gameInput->d.isDown) { gameGlobal->offsetY -= scrollSpeed; }
    if (gameInput->e.isDown) { gameGlobal->offsetY += scrollSpeed; }
    
    uint32_t pitch = gameBuffer->backBuffer.width * gameBuffer->backBuffer.bytesPerPixel;
    uint8_t  *row  = (uint8_t *)gameBuffer->backBuffer.textureMemory;
    for (int y = 0; y < gameBuffer->backBuffer.height; y++) {
        uint8_t *pixel = (uint8_t *)row;
        for (int x = 0; x < gameBuffer->backBuffer.width; x++) {
            *pixel = (uint8_t)(x + gameGlobal->offsetX);
            pixel++;
            *pixel = (uint8_t)(y + gameGlobal->offsetY);
            pixel++;
            *pixel = 0;
            pixel++;
            *pixel = 255;
            pixel++;
        }
        row += pitch;
    }
}

void audioTestDEBUG(GameInput *gameInput, GameGlobal *gameGlobal, GameSound *gameSound)
{
#if 0
    gameGlobal->toneHz = 256 +
        (uint32_t)(512.0f*(  (float)gameInput->controller[0].leftTrigger
                           / (float)CONTR_AXIS_NORMALIZATION            ));
#else
    gameGlobal->toneHz += 8 * gameInput->mouseScrV;
    if (gameGlobal->toneHz < 64)  { gameGlobal->toneHz = 64; }
    if (gameGlobal->toneHz > 512) { gameGlobal->toneHz = 512; }
#endif
    gameGlobal->wavePeriod = AUDIO_SAMPLES_PER_SECOND / gameGlobal->toneHz;
    gameGlobal->halfWavePeriod = gameGlobal->wavePeriod / 2;

    gameSound->audioToQueueBytes = gameSound->targetQueuedBytes - gameSound->queuedBytes;
    if (gameSound->audioToQueueBytes > sizeof(gameSound->audioToQueue)) {
        printf("%s out of bounds of audioToQueue\n", __FUNCTION__);
    }
    // printf("target: %u, queued: %u\n", gameSound->targetQueuedBytes,
    //                                    gameSound->queuedBytes       );

    if (gameSound->audioToQueueBytes) {
        int16_t *sampleOut   = gameSound->audioToQueue;
        uint32_t sampleCount =   gameSound->audioToQueueBytes
                               /(gameSound->bytesPerSamplePerChannel*AUDIO_CHANNELS);

        for (uint32_t i = 0; i < sampleCount; i++) {
            int16_t sampleValue = gameGlobal->toneVolume;
#ifdef SQUARE_WAVE_TEST
            if ((gameGlobal.runningSampleIndex / gameGlobal.halfWavePeriod) % 2 == 0) {
                sampleValue *= -1;
            }
#else 
            gameGlobal->tWave = 2.0f*PI32 * ( (float)gameGlobal->runningSampleIndex
                                             /(float)gameGlobal->wavePeriod        );
            if (gameGlobal->tWave > 2.0f*PI32) {
                gameGlobal->tWave -= 2.0f*PI32;
            }
            float sineValue = sinf(gameGlobal->tWave);
            sampleValue *= sineValue;
#endif
            gameGlobal->runningSampleIndex++;
            for (uint32_t j = 0; j < AUDIO_CHANNELS; j++) {
                *sampleOut = sampleValue;
                sampleOut++;
            }
            
        }
    }
}

void gameUpdate(GameState *gameState, GameMemory *gameMemory)
{
    if (gameState->gameInput.esc.isDown) {
        gameState->gameGlobal.quitGame = true;
        return;
    }

    // printf("%.02f ms/f, %.02f f/s, %lu cycles\n", gameState->gameClocks.msPerFrame,
    //                                               gameState->gameClocks.framesPerSecond,
    //                                               gameState->gameClocks.elapsedCycleCount);

    inputTestDEBUG(&gameState->gameInput);

    textureTestDEBUG(&gameState->gameInput, &gameState->gameGlobal,
                     &gameState->gameBuffer, &gameState->gameClocks);

    audioTestDEBUG(&gameState->gameInput, &gameState->gameGlobal, &gameState->gameSound);

    int n = 0;
    // for (int i = 0; i < 1000; i++) {
    //     for (int j = 0; j < 5000; j++) {
    //         n = i*j;
    //     }
    // }

    gameState->gameGlobal.gameFrame++;
}
