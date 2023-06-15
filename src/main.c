#include <math.h>
#include <time.h>
#include "raylib.h"
#include "util.h"
#include "random_note_player.h"
#include "sound_pool.h"
#include "thread_list.h"
#include "text.h"

#define LEVEL_RANGES_LENGTH 9
#define COLOR_PALETTES_LENGTH 5
#define KALIMBA_SOUNDS_LENGTH 3
#define LOADED_WAVES_LENGTH (KALIMBA_SOUNDS_LENGTH + 1)

#define MAX_LEVEL 40

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
    Circle playerCircle;
    Circle targetCircle;
    float shrinkingSpeed;
} Game;

void Init(void);
void Cleanup(void);
void UpdateDrawFrame(void);

void startGame(void);
void advanceLevel(void);
void endGame(GameState won);
void updateUI(void);
void initLevelRadiusRanges(void);
void initColorPalettes(void);
    
Dimensions screenDimensions;
int fontSizeMedium = 72;
int fontSizeBig = 432; // 72 * 6
Game game;
Range levelCircleRadiusRanges[LEVEL_RANGES_LENGTH];
ColorPalette colorPalettes[COLOR_PALETTES_LENGTH];
SoundPool kalimbaSoundPools[KALIMBA_SOUNDS_LENGTH];
SoundPool echoSoundPool;
Texture2D circleTexture;
Font fontMedium;
Font fontBig;
Text controlsText;
Text endText;
Text scoreText;

int main(void) {
    Init();
    
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    Cleanup();
    return 0;
}

#include <stdio.h>
void Init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shrunk!");
    SetWindowState(FLAG_VSYNC_HINT);
    screenDimensions = toggleGameFullscreen();
    
    HideCursor();
    
    // Init audio device and create the sound loading threads
    InitAudioDevice();

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
    
    initRandom();
    initColorPalettes();
    initLevelRadiusRanges();

    game.currentColorPalette = colorPalettes[game.level / 10];
    game.gameState = GAMESTATE_SHRUNK;
    game.level = 0;
    game.shrinkingSpeed = 0.0f;
    
    fontMedium = LoadFontEx("resources/fonts/Nunito/Nunito-Regular.ttf", fontSizeMedium, NULL, 0);
    fontBig = LoadFontEx("resources/fonts/Quicksand/Quicksand-Bold.ttf", fontSizeBig, NULL, 0);

    Text_InitText(&controlsText,
        &screenDimensions,
        &fontMedium, 
        "[Click] to play, [Esc] to exit",
        ALIGN_CENTER_BOTTOM,
        ColorAlpha(game.currentColorPalette.foreground, 0.25f));

    Text_InitText(&endText,
        &screenDimensions,
        &fontMedium, 
        "Shrunk",
        ALIGN_CENTER,
        ColorAlpha(game.currentColorPalette.foreground, 0.75f));

    Text_InitText(&scoreText,
        &screenDimensions,
        &fontBig, 
        TextFormat("%d", game.level),
        ALIGN_CENTER,
        ColorAlpha(game.currentColorPalette.foreground, 0.125f));
    
    float circleRadius = levelCircleRadiusRanges[0].max;
    circleTexture = LoadTexture("resources/textures/circle.png");
    SetTextureFilter(circleTexture, TEXTURE_FILTER_TRILINEAR);
    game.targetCircle = (Circle) { .texture = circleTexture };
    game.playerCircle = (Circle) { 
        .texture = circleTexture,
        .radius = circleRadius,
        .color = ColorAlpha(game.currentColorPalette.foreground, 0.5f)
    };
    
    ThreadList_Join(threads);
    ThreadList_Delete(threads);
    for (int i = 0 ; i < LOADED_WAVES_LENGTH ; i++) {
        UnloadWave(loadedWaves[i]);
    }
    RandomNotePlayer_Init(kalimbaSoundPools);
}

void Cleanup() {
    for (int i = 0 ; i < KALIMBA_SOUNDS_LENGTH ; i++) {
        SoundPool_UnloadSound(&kalimbaSoundPools[i]);
    }
    SoundPool_UnloadSound(&echoSoundPool);
    UnloadTexture(circleTexture);
    UnloadFont(fontMedium);
    UnloadFont(fontBig);

    CloseAudioDevice();
    CloseWindow();
}

void UpdateDrawFrame() {
    if (IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))) {
        screenDimensions = toggleGameFullscreen();
        updateUI();
    }
    if (IsCursorOnScreen())
        game.playerCircle.position = GetMousePosition();

    if (game.gameState == GAMESTATE_PLAYING) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool collision = CheckCollisionCircles(game.playerCircle.position, game.playerCircle.radius, game.targetCircle.position, game.targetCircle.radius);
            if (collision) {
                advanceLevel();
                if (game.level > MAX_LEVEL) {
                    endGame(GAMESTATE_WON);
                }
            } else {
                endGame(GAMESTATE_MISSED);
            }
        }

        float deltaTime = GetFrameTime();
        game.playerCircle.radius -= game.shrinkingSpeed * deltaTime;
        game.targetCircle.radius -= game.shrinkingSpeed * deltaTime;

        if (game.playerCircle.radius <= 0.0f) {
            endGame(GAMESTATE_SHRUNK);
        }
        BeginDrawing();
        ClearBackground(game.currentColorPalette.background);
        drawCircle(game.playerCircle);
        drawCircle(game.targetCircle);
        Text_DrawText(scoreText);

        EndDrawing();
    } else {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            startGame();
        }

        BeginDrawing();
        ClearBackground(game.currentColorPalette.background);
        drawCircle(game.playerCircle);
        Text_DrawText(endText);
        Text_DrawText(controlsText);
        EndDrawing();
    }
}

void startGame() {
    game.gameState = GAMESTATE_PLAYING;
    game.level = 1;
    scoreText.text = TextFormat("%d", game.level);
    Text_Update(&scoreText);
    game.currentColorPalette = colorPalettes[game.level / 10];
    scoreText.color = ColorAlpha(game.currentColorPalette.foreground, 0.125f);
    float circleRadius = levelCircleRadiusRanges[0].max;
    
    game.playerCircle.radius = circleRadius;
    game.playerCircle.color = ColorAlpha(game.currentColorPalette.foreground, 0.5f);
    
    game.targetCircle.radius = circleRadius;
    game.targetCircle.color = ColorAlpha(game.currentColorPalette.foreground, 0.25f);
    game.targetCircle.position = (Vector2) { .x = screenDimensions.width / 2, .y = screenDimensions.height / 2};

    game.shrinkingSpeed = 2.0f;

    RandomNotePlayer_Reset();
    game.gameStartTime = GetTime();
}

void advanceLevel() {
    Note note = RandomNotePlayer_GenerateNote(game.level, MAX_LEVEL);
    SoundPool_PlaySound(note.soundPool, note.pitch);
    SoundPool_PlaySound(&echoSoundPool, note.pitch);
    if (game.level % 10 == 0) {
        RandomNotePlayer_RandomizeMode();
    }
    game.level++;
    scoreText.text = TextFormat("%d", game.level);
    Text_Update(&scoreText);

    game.shrinkingSpeed += 1.0f;
    game.currentColorPalette = colorPalettes[game.level / 10];
    scoreText.color = ColorAlpha(game.currentColorPalette.foreground, 0.125f);

    Range levelRange = levelCircleRadiusRanges[game.level / 5];
    float randomRadius = GetRandomValueF(levelRange.min, levelRange.max);

    game.playerCircle.color = ColorAlpha(game.currentColorPalette.foreground, 0.5f);
    game.playerCircle.radius = randomRadius;

    game.targetCircle.color = ColorAlpha(game.currentColorPalette.foreground, 0.25f);
    game.targetCircle.radius = randomRadius;
    game.targetCircle.position = (Vector2) {
        .x = GetRandomValue(0, screenDimensions.width), 
        .y = GetRandomValue(0, screenDimensions.height)
    };
}

void endGame(GameState newGameState) {
    double gameTime = GetTime() - game.gameStartTime;
    game.gameState = newGameState;
    if (game.gameState == GAMESTATE_WON) {
        endText.text = TextFormat("You won in %.2f seconds!", gameTime);
    } else if (game.gameState == GAMESTATE_SHRUNK) {
        endText.text = TextFormat("Shrunk!");
    } else if (game.gameState == GAMESTATE_MISSED) {
        endText.text = TextFormat("Missed!");
    }
    Text_Update(&endText);
    endText.color = ColorAlpha(game.currentColorPalette.foreground, 0.75f);
    controlsText.color = ColorAlpha(game.currentColorPalette.foreground, 0.25f);
}

void updateUI() {
    Text_Update(&scoreText);
    Text_Update(&controlsText);
    Text_Update(&endText);
}

void initColorPalettes() {
    // 01 - 10: Green
    colorPalettes[0] = (ColorPalette) { .background = GetColor(0x496F5DFF), .foreground = GetColor(0xC0DFA1FF) };
    // 11 - 20: Brown
    colorPalettes[1] = (ColorPalette) { .background = GetColor(0xA88383FF), .foreground = GetColor(0xFFF3EDFF) };
    // 21 - 30: Julia
    colorPalettes[2] = (ColorPalette) { .background = GetColor(0xAB4E68FF), .foreground = GetColor(0xFFF9E7FF) };
    // 31 - 39: Moon
    colorPalettes[3] = (ColorPalette) { .background = GetColor(0x39304AFF), .foreground = GetColor(0xEDD2E0FF) };
    // 40: Moon
    colorPalettes[4] = (ColorPalette) { .background = GetColor(0x39304AFF), .foreground = GetColor(0xEDD2E0FF) };
}

void initLevelRadiusRanges() {
    // Defines the player and target radius min and max shared value.
    // starting at 150:135 and decreasing 15 every 5 levels.
    int value = 150;
    for (int i = 0 ; i < LEVEL_RANGES_LENGTH - 1 ; i++) {
        levelCircleRadiusRanges[i].max = value;
        value -= 13.5;
        levelCircleRadiusRanges[i].min = value;
    }
    // In level 40 specifically, the radius is set to 30, the smallest
    // radius in the game.
    levelCircleRadiusRanges[LEVEL_RANGES_LENGTH - 1].max = value;
    levelCircleRadiusRanges[LEVEL_RANGES_LENGTH - 1].min = value;
}