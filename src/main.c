#include <math.h>
#include <time.h>
#include "raylib.h"
#include "util.h"
#include "kalimba.h"
#include "sound_pool.h"
#include "thread_list.h"

#define LEVEL_RANGES_LENGTH 9
#define COLOR_PALETTES_LENGTH 5
#define KALIMBA_SOUNDS_LENGTH 3
#define LOADED_WAVES_LENGTH (KALIMBA_SOUNDS_LENGTH + 1)

#define MAX_LEVEL 40

Dimensions screenDimensions = (Dimensions) { 
    width: SCREEN_WIDTH, 
    height: SCREEN_HEIGHT
};

typedef enum {
    GAMESTATE_PLAYING,
    GAMESTATE_WON,
    GAMESTATE_SHRUNK,
    GAMESTATE_MISSED
} GameState;

typedef struct Game {
    GameState gameState;
    double gameStartTime;
    int level;
    ColorPalette currentColorPalette;
    const char* endGameText;
    float shrinkingSpeed;
} Game;

void advanceLevel(Game* game, Circle* playerCircle, Circle* targetCircle);
void startGame(Game* game, Circle* playerCircle, Circle* targetCircle);
void endGame(Game* game, GameState won);
void initLevelRadiusRanges(void);
void initColorPalettes(void);

Range levelCircleRadiusRanges[LEVEL_RANGES_LENGTH];
ColorPalette colorPalettes[COLOR_PALETTES_LENGTH];

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shrunk!");
    SetWindowState(FLAG_VSYNC_HINT);
    screenDimensions = toggleGameFullscreen();
    
    // Draw once to change background to the green background
    // color instead of white
    BeginDrawing();
    ClearBackground(GetColor(0x496F5DFF));
    EndDrawing();

    // Init audio device and create the sound loading threads
    InitAudioDevice();
    SoundPool kalimbaSoundPools[KALIMBA_SOUNDS_LENGTH];
    SoundPool echoSoundPool;

    Wave loadedWaves[LOADED_WAVES_LENGTH];
    ThreadList* threads = ThreadList_New();
    int waveIndex = 0;
    while (waveIndex < KALIMBA_SOUNDS_LENGTH) {
        loadedWaves[waveIndex] = LoadWave(TextFormat("resources/sounds/Kalimba-C%d.wav", (waveIndex + 4)));
        SoundPool_LoadSoundFromWave(&kalimbaSoundPools[waveIndex], loadedWaves[waveIndex], threads);
        waveIndex++;
    }
    loadedWaves[waveIndex] = LoadWave("resources/sounds/Echo.wav");
    SoundPool_LoadSoundFromWave(&echoSoundPool, loadedWaves[waveIndex], threads);
    HideCursor();
    initRandom();
    initColorPalettes();
    initLevelRadiusRanges();
    
    int fontSize = 72;
    int bigFontSize = fontSize * 6;
    Font font = LoadFontEx("resources/fonts/Nunito/Nunito-Regular.ttf", fontSize, NULL, 0);
    Font bigFont = LoadFontEx("resources/fonts/Quicksand/Quicksand-Bold.ttf", bigFontSize, NULL, 0);

    Texture2D circleTexture = LoadTexture("resources/textures/circle.png");
    SetTextureFilter(circleTexture, TEXTURE_FILTER_TRILINEAR);
    Circle playerCircle = (Circle) { texture: circleTexture };
    Circle targetCircle = (Circle) { texture: circleTexture };
    
    ThreadList_Join(threads);
    ThreadList_Delete(threads);
    Kalimba_Init(kalimbaSoundPools, &echoSoundPool);
    Game game;
    startGame(&game, &playerCircle, &targetCircle);
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))) {
            screenDimensions = toggleGameFullscreen();
        }
        if (IsCursorOnScreen())
            playerCircle.position = GetMousePosition();

        if (game.gameState == GAMESTATE_PLAYING) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                bool collision = CheckCollisionCircles(playerCircle.position, playerCircle.radius, targetCircle.position, targetCircle.radius);
                if (collision) {
                    advanceLevel(&game, &playerCircle, &targetCircle);
                    if (game.level > MAX_LEVEL) {
                        endGame(&game, GAMESTATE_WON);
                    }
                } else {
                    endGame(&game, GAMESTATE_MISSED);
                }
            }

            float deltaTime = GetFrameTime();
            playerCircle.radius -= game.shrinkingSpeed * deltaTime;
            targetCircle.radius -= game.shrinkingSpeed * deltaTime;

            if (playerCircle.radius <= 0.0f) {
                endGame(&game, GAMESTATE_SHRUNK);
            }
            BeginDrawing();
            ClearBackground(game.currentColorPalette.background);
            drawCircle(playerCircle);
            drawCircle(targetCircle);
            Vector2 textSize = MeasureTextEx(bigFont, TextFormat("%d", game.level), bigFontSize, 0);
            Vector2 textPosition = (Vector2) { x: screenDimensions.width / 2 - textSize.x / 2, y: screenDimensions.height / 2 - textSize.y / 2};
            Color textColor = ColorAlpha(game.currentColorPalette.foreground, 0.125f);
            DrawTextEx(bigFont, TextFormat("%d", game.level), textPosition, bigFontSize, 0, textColor);
            EndDrawing();
        } else {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                startGame(&game, &playerCircle, &targetCircle);
            }

            BeginDrawing();
            ClearBackground(game.currentColorPalette.background);
            drawCircle(playerCircle);
            Vector2 textSize = MeasureTextEx(font, game.endGameText, fontSize, 0);
            Vector2 textPosition = (Vector2) { x: screenDimensions.width / 2 - textSize.x / 2, y: screenDimensions.height / 2 - textSize.y / 2};
            Color textColor = ColorAlpha(game.currentColorPalette.foreground, 0.75f);
            DrawTextEx(font, game.endGameText, textPosition, fontSize, 0, textColor);
            EndDrawing();
        }
    }

    for (int i = 0 ; i < KALIMBA_SOUNDS_LENGTH ; i++) {
        SoundPool_UnloadSound(&kalimbaSoundPools[i]);
    }
    SoundPool_UnloadSound(&echoSoundPool);

    for (int i = 0 ; i < LOADED_WAVES_LENGTH ; i++) {
        UnloadWave(loadedWaves[i]);
    }
    
    UnloadTexture(circleTexture);
    
    UnloadFont(font);
    UnloadFont(bigFont);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void advanceLevel(Game* game, Circle* playerCircle, Circle* targetCircle) {
    Kalimba_Play(game->level, MAX_LEVEL);
    if (game->level % 10 == 0) {
        Kalimba_RandomizeMode();
    }
    game->level++;

    game->shrinkingSpeed += 1.0f;
    game->currentColorPalette = colorPalettes[game->level / 10];

    Range levelRange = levelCircleRadiusRanges[game->level / 5];
    float randomRadius = GetRandomValueF(levelRange.min, levelRange.max);

    playerCircle->color = ColorAlpha(game->currentColorPalette.foreground, 0.5f);
    playerCircle->radius = randomRadius;

    targetCircle->color = ColorAlpha(game->currentColorPalette.foreground, 0.25f);
    targetCircle->radius = randomRadius;
    targetCircle->position = (Vector2) {
        x: GetRandomValue(0, screenDimensions.width), 
        y: GetRandomValue(0, screenDimensions.height)
    };
}

void startGame(Game* game, Circle* playerCircle, Circle* targetCircle) {
    game->gameState = GAMESTATE_PLAYING;
    game->level = 1;
    game->currentColorPalette = colorPalettes[game->level / 10];
    float circleRadius = levelCircleRadiusRanges[0].max;
    
    playerCircle->radius = circleRadius;
    playerCircle->color = ColorAlpha(game->currentColorPalette.foreground, 0.5f);
    
    targetCircle->radius = circleRadius;
    targetCircle->color = ColorAlpha(game->currentColorPalette.foreground, 0.25f);
    targetCircle->position = (Vector2) { x: screenDimensions.width / 2, y: screenDimensions.height / 2};

    game->shrinkingSpeed = 2.0f;

    Kalimba_Reset();
    game->gameStartTime = GetTime();
}

void endGame(Game* game, GameState newGameState) {
    double gameTime = GetTime() - game->gameStartTime;
    game->gameState = newGameState;
    if (game->gameState == GAMESTATE_WON) {
        game->endGameText = TextFormat("You won in %.2f seconds!", gameTime);
    } else if (game->gameState == GAMESTATE_SHRUNK) {
        game->endGameText = TextFormat("Shrunk!");
    } else if (game->gameState == GAMESTATE_MISSED) {
        game->endGameText = TextFormat("Missed!");
    }
}

void initColorPalettes() {
    // 01 - 10: Green
    colorPalettes[0] = (ColorPalette) { background: GetColor(0x496F5DFF), foreground: GetColor(0xC0DFA1FF) };
    // 11 - 20: Brown
    colorPalettes[1] = (ColorPalette) { background: GetColor(0xA88383FF), foreground: GetColor(0xFFF3EDFF) };
    // 21 - 30: Julia
    colorPalettes[2] = (ColorPalette) { background: GetColor(0xAB4E68FF), foreground: GetColor(0xFFF9E7FF) };
    // 31 - 39: Moon
    colorPalettes[3] = (ColorPalette) { background: GetColor(0x39304AFF), foreground: GetColor(0xEDD2E0FF) };
    // 40: Moon
    colorPalettes[4] = (ColorPalette) { background: GetColor(0x39304AFF), foreground: GetColor(0xEDD2E0FF) };
}

void initLevelRadiusRanges() {
    // Defines the player and target radius min and max shared value.
    // starting at 150:135 and decreasing 15 every 5 levels.
    int value = 150;
    for (int i = 0 ; i < LEVEL_RANGES_LENGTH - 1 ; i++) {
        levelCircleRadiusRanges[i].max = value;
        value -= 15;
        levelCircleRadiusRanges[i].min = value;
    }
    // In level 40 specifically, the radius is set to 30, the smallest
    // radius in the game.
    levelCircleRadiusRanges[LEVEL_RANGES_LENGTH - 1].max = value;
    levelCircleRadiusRanges[LEVEL_RANGES_LENGTH - 1].min = value;
}