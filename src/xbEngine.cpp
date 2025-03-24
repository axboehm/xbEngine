#include "xbEngine.h"
#include "xbMath.h"
#include "constants.h"

#include <cstdio> // for printf
#include <math.h> // for sinf

// get the position (id) of a specific key in GameInput->keys
uint32_t getKeyID(ButtonState *buttonState, GameInput *gameInput) {
    uint32_t id = buttonState - &gameInput->keys[0];
    printf("ID of pressed key: %u\n", id);
    return id;
}

void inputTestDEBUG(GameInput *gameInput, GameBuffer *gameBuffer)
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
        uint32_t color  = 0xFF141478;
        uint32_t row    = 0;
        uint32_t column = 0;
        if        (i < (&gameInput->backQuote - &gameInput->keys[0])) {
            column = i;
        } else if (   i >= (&gameInput->backQuote - &gameInput->keys[0])
                   && i <  (&gameInput->tab       - &gameInput->keys[0])) { 
            row = 1;
            column = i - (&gameInput->backQuote - &gameInput->keys[0]);
        } else if (   i >= (&gameInput->tab  - &gameInput->keys[0])
                   && i <  (&gameInput->caps - &gameInput->keys[0])) {
            row = 2;
            column = i - (&gameInput->tab - &gameInput->keys[0]);
        } else if (   i >= (&gameInput->caps   - &gameInput->keys[0])
                   && i <  (&gameInput->lShift - &gameInput->keys[0])) {
            row = 3;
            column = i - (&gameInput->caps - &gameInput->keys[0]);
        } else if (   i >= (&gameInput->lShift - &gameInput->keys[0])
                   && i <  (&gameInput->lCtrl  - &gameInput->keys[0])) {
            row = 4;
            column = i - (&gameInput->lShift - &gameInput->keys[0]);
        } else if (i >= (&gameInput->lCtrl     - &gameInput->keys[0])) {
            row = 5;
            column = i - (&gameInput->lCtrl - &gameInput->keys[0]);
        }

        if (gameInput->keys[i].isDown) {
            //printf("Key %u held down\n", i);
            color = 0xFF2299FF;
        }

        float cornerOffset = 50.0f;
        float keyThickness = 30.0f;
        float keySpacing   = 10.0f;
        float startX = cornerOffset + column*keySpacing + column*keyThickness;
        float endX   = startX + keyThickness;
        float startY = cornerOffset + row*keySpacing + row*keyThickness;
        float endY   = startY + keyThickness;
        drawRectangle(startX, startY, endX, endY, gameBuffer, color);
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
    if (gameInput->s.isDown || gameInput->left.isDown ) { gameTest->offsetX -= scrollSpeed; }
    if (gameInput->f.isDown || gameInput->right.isDown) { gameTest->offsetX += scrollSpeed; }
    if (gameInput->d.isDown || gameInput->down.isDown ) { gameTest->offsetY -= scrollSpeed; }
    if (gameInput->e.isDown || gameInput->up.isDown   ) { gameTest->offsetY += scrollSpeed; }
    
    uint32_t pitch = gameBuffer->width * gameBuffer->bytesPerPixel;
    uint8_t  *row  = (uint8_t *)gameBuffer->textureMemory;
    for (int y = 0; y < gameBuffer->height; y++) {
        uint8_t *pixel = (uint8_t *)row;
        for (int x = 0; x < gameBuffer->width; x++) {
            uint8_t value = (uint8_t)y+gameTest->offsetY;
            if (x % 256 == 0 || y % 256 == 0) { value = 0; }
            *pixel = value; // blue
            pixel++;
            *pixel = value; // green
            pixel++;
            *pixel = value; // red
            pixel++;
            *pixel = 255; // alpha
            pixel++;
        }
        row += pitch;
    }
}

void audioTestDEBUG(GameInput *gameInput, GameTest *gameTest,
                    GameSound *gameSound, GameClocks *gameClocks, GameBuffer *gameBuffer)
{
    float toneMult = 1.0f - (((float)gameInput->mousePosY) / ((float)gameBuffer->height));
          toneMult = clampF32(toneMult, 0.0f, 1.0f); // necessary when resizing window
    float toneHz   = 64.0f + 512.0f * toneMult;
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

void mouseTestDEBUG(GameInput *gameInput, GameBuffer *gameBuffer)
{
    float colorMult = ((float)gameInput->mousePosY) / ((float)gameBuffer->height);
    uint8_t red   = lerpU8(0x00, 0xFF, colorMult);
    uint8_t green = lerpU8(0x00, 0xFF, 1.0f-colorMult);
    uint32_t mouseVisColor = 0xFF000000 | (red << 16) | (green << 8); // AARRGGBB
    
    float rectThickness = 10.0f;
    drawRectangle(gameInput->mousePosX - rectThickness, gameInput->mousePosY - rectThickness,
                  gameInput->mousePosX + rectThickness, gameInput->mousePosY + rectThickness,
                  gameBuffer, mouseVisColor);
}

// will draw a rectangle between rounded pixel coordinates in specified color
// will draw from start pixel coordinates up to but not including end pixel coordinates
// this should allow to draw perfectly touching rectangles even considering sub pixel positioning
void drawRectangle(float startXF, float startYF, float endXF, float endYF,
                   GameBuffer *gameBuffer, uint32_t color                 )
{
    int32_t startX = roundF32toI32(startXF);
    int32_t startY = roundF32toI32(startYF);
    int32_t endX   = roundF32toI32(endXF);
    int32_t endY   = roundF32toI32(endYF);

    //NOTE[ALEX]: true clamping is not required, as the for loop takes care of the remaining cases
    startX = maxI32(startX, 0.0f);
    startY = maxI32(startY, 0.0f);
    endX   = minI32(endX, (float)gameBuffer->width);
    endY   = minI32(endY, (float)gameBuffer->height);

    // printf("%s from (%i, %i) to (%i, %i)\n", __FUNCTION__, startX, startY, endX, endY);

    uint8_t *row = (uint8_t *)gameBuffer->textureMemory
                     + startX * gameBuffer->bytesPerPixel
                     + startY * gameBuffer->pitch;
    for (int j = startY; j < endY; j++) {
        uint32_t *pixel = (uint32_t *)row;
        for (int i = startX; i < endX; i++) {
            *pixel = color;
            pixel++;
        }
        row += gameBuffer->pitch;
    }
}

void gameUpdate(GameState *gameState, GameTest *gameTest)
{
    GameGlobal *gameGlobal = &gameState->gameGlobal;
    GameInput  *gameInput  = &gameState->gameInput;
    GameClocks *gameClocks = &gameState->gameClocks;
    GameBuffer *gameBuffer = &gameState->gameBuffer;
    GameSound  *gameSound  = &gameState->gameSound;

    if (gameInput->esc.transitionCount > 1) {
        gameInput->esc.transitionCount %= 2;
        gameGlobal->quitGame = true;
        return;
    }

    textureTestDEBUG(gameInput, gameTest, gameBuffer, gameClocks);

    inputTestDEBUG(gameInput, gameBuffer);

    audioTestDEBUG(gameInput, gameTest, gameSound, gameClocks, gameBuffer);

    mouseTestDEBUG(gameInput, gameBuffer);

    gameGlobal->gameFrame++;
}
