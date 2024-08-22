#include "util.h"

#include <stddef.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

extern Dimensions screenDimensions;

void drawCircle(Circle circle) {
    Rectangle source = (Rectangle) { .x = 0.0f, .y = 0.0f, .width = circle.texture.width, .height = circle.texture.height };
    Rectangle dest = (Rectangle) { .x = circle.position.x, .y = circle.position.y, .width = circle.radius * 2, .height = circle.radius * 2 };
    Vector2 origin = (Vector2) { .x = circle.radius, .y = circle.radius };
    DrawTexturePro(circle.texture, source, dest, origin, 0, circle.color);
}

void toggleWindowFullscreen() {
    if (IsWindowFullscreen()) {
        screenDimensions.width = SCREEN_WIDTH;
        screenDimensions.height = SCREEN_HEIGHT;
        ToggleFullscreen();
        SetWindowSize(screenDimensions.width, screenDimensions.height);
    } else {
        int monitor = GetCurrentMonitor();
        screenDimensions.width = GetMonitorWidth(monitor);
        screenDimensions.height = GetMonitorHeight(monitor);
        SetWindowSize(screenDimensions.width, screenDimensions.height);
        ToggleFullscreen();
    }
}

void initRandom() {
    SetRandomSeed(time(NULL));
}

float GetRandomValueF(float min, float max) {
    return min + (float) GetRandomValue(0, RAYLIB_RAND_MAX) / (float) (RAYLIB_RAND_MAX / 1.0f) * (max - min);
}

double GetRandomValueD(double min, double max) {
    return min + (double) GetRandomValue(0, RAYLIB_RAND_MAX) / (double) (RAYLIB_RAND_MAX / 1.0) * (max - min);
}

float easeInCirc(float x) {
    return 1 - sqrt(1 - pow(x, 2));
}

float easeOutCirc(float x) {
    return sqrt(1 - pow(x - 1, 2));
}