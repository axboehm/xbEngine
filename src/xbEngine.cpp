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
}

void textureTestDEBUG(GameInput *gameInput, GameTest *gameTest,
                      GameBuffer *gameBuffer, GameClocks *gameClocks)
{
    uint32_t scrollSpeed = 8; //NOTE[ALEX]: framerate dependent
    gameTest->offsetX += 
        (int16_t)(scrollSpeed *   (float)gameInput->controller[0].leftStickX
                                / (float)CONTR_AXIS_NORMALIZATION           );
    gameTest->offsetY +=
        (int16_t)(scrollSpeed *   (float)gameInput->controller[0].leftStickY
                                / (float)CONTR_AXIS_NORMALIZATION           );
    if (gameInput->s.isDown) { gameTest->offsetX -= scrollSpeed; }
    if (gameInput->f.isDown) { gameTest->offsetX += scrollSpeed; }
    if (gameInput->d.isDown) { gameTest->offsetY -= scrollSpeed; }
    if (gameInput->e.isDown) { gameTest->offsetY += scrollSpeed; }
    
    uint32_t pitch = gameBuffer->width * gameBuffer->bytesPerPixel;
    uint8_t  *row  = (uint8_t *)gameBuffer->textureMemory;
    for (int y = 0; y < gameBuffer->height; y++) {
        uint8_t *pixel = (uint8_t *)row;
        for (int x = 0; x < gameBuffer->width; x++) {
            *pixel = (uint8_t)(x + gameTest->offsetX);
            pixel++;
            *pixel = (uint8_t)(y + gameTest->offsetY);
            pixel++;
            *pixel = 0;
            pixel++;
            *pixel = 255;
            pixel++;
        }
        row += pitch;
    }
}

void audioTestDEBUG(GameInput *gameInput, GameTest *gameTest,
                    GameSound *gameSound, GameClocks *gameClocks, GameBuffer *gameBuffer)
{
    float toneHz = 64.0f + 512.0f * (((float)gameInput->mousePosY) / ((float)gameBuffer->height));
    gameTest->toneHz = (uint32_t)toneHz;

    gameTest->wavePeriod = AUDIO_SAMPLES_PER_SECOND / gameTest->toneHz;
    gameTest->halfWavePeriod = gameTest->wavePeriod / 2;

    // if the framerate drops, the audio gets choppy
    gameSound->audioToQueueBytes = gameSound->targetQueuedBytes - gameSound->queuedBytes;
    if (gameSound->audioToQueueBytes > sizeof(gameSound->audioToQueue)) {
        printf("%s out of bounds of audioToQueue\n", __FUNCTION__);
    }

    if (gameSound->audioToQueueBytes) {
        int16_t *sampleOut   = gameSound->audioToQueue;
        uint32_t sampleCount =   gameSound->audioToQueueBytes
                               /(gameSound->bytesPerSamplePerChannel*AUDIO_CHANNELS);

        for (uint32_t i = 0; i < sampleCount; i++) {
            int16_t sampleValue = gameTest->toneVolume;

            gameTest->tWave += 2.0f*PI32 *(1.0f / (float)gameTest->wavePeriod);
            if (gameTest->tWave > 2.0f*PI32) {
                gameTest->tWave -= 2.0f*PI32;
            }
            float sineValue = sinf(gameTest->tWave);
            sampleValue *= sineValue;

            gameTest->runningSampleIndex++;
            for (uint32_t j = 0; j < AUDIO_CHANNELS; j++) {
                *sampleOut = sampleValue;
                sampleOut++;
            }
            
        }
    }

    // printf("target: %u, currently queued: %u, to queue: %u, time last frame: %.04fms\n",
    //        gameSound->targetQueuedBytes, gameSound->queuedBytes,
    //        gameSound->audioToQueueBytes, gameClocks->msLastFrame                        );
}

void drawRectangle(int startX, int startY, int endX, int endY,
                   GameBuffer *gameBuffer, uint32_t color       )
{
    if (startX < 0) { startX = 0; }
    if (startY < 0) { startY = 0; }
    if (endX > gameBuffer->width)  { endX = gameBuffer->width; }
    if (endY > gameBuffer->height) { endY = gameBuffer->height; }
    for (int i = startX; i < endX; i++) {
        for (int j = startY; j < endY; j++) {
            uint32_t *pixel = (uint32_t *)gameBuffer->textureMemory + i + j*gameBuffer->width;
            *pixel = color;
        }
    }
}

void gameUpdate(GameState *gameState, GameTest *gameTest)
{
    GameInput  *gameInput  = &gameState->gameInput;
    GameClocks *gameClocks = &gameState->gameClocks;
    GameBuffer *gameBuffer = &gameState->gameBuffer;
    GameSound  *gameSound  = &gameState->gameSound;

    if (gameState->gameInput.esc.isDown) {
        gameState->gameGlobal.quitGame = true;
        return;
    }

    inputTestDEBUG(gameInput);

    textureTestDEBUG(gameInput, gameTest, gameBuffer, gameClocks);

    int rectThickness = 10;
    drawRectangle(gameInput->mousePosX - rectThickness, gameInput->mousePosY - rectThickness,
                  gameInput->mousePosX + rectThickness, gameInput->mousePosY + rectThickness,
                  gameBuffer, 0xFFFFFFFF);

    audioTestDEBUG(gameInput, gameTest, gameSound, gameClocks, gameBuffer);

    // int n = 0;
    // for (int i = 0; i < 1000; i++) {
    //     for (int j = 0; j < 5000; j++) {
    //         n = i*j;
    //     }
    // }
    // printf("%i\n", n);

    gameState->gameGlobal.gameFrame++;
}
