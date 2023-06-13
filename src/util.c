#include "util.h"

#include <stddef.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

void drawCircle(Circle circle) {
    Rectangle source = (Rectangle) { .x = 0.0f, .y = 0.0f, .width = circle.texture.width, .height = circle.texture.height };
    Rectangle dest = (Rectangle) { .x = circle.position.x, .y = circle.position.y, .width = circle.radius * 2, .height = circle.radius * 2 };
    Vector2 origin = (Vector2) { .x = circle.radius, .y = circle.radius };
    DrawTexturePro(circle.texture, source, dest, origin, 0, circle.color);
}

Dimensions toggleGameFullscreen() {
    Dimensions newDimensions;
    if (IsWindowFullscreen()) {
        newDimensions.width = SCREEN_WIDTH;
        newDimensions.height = SCREEN_HEIGHT;
    } else {
        int monitor = GetCurrentMonitor();
        newDimensions.width = GetMonitorWidth(monitor);
        newDimensions.height = GetMonitorHeight(monitor);
    }
    ToggleFullscreen();
    SetWindowSize(newDimensions.width, newDimensions.height);
    return newDimensions;
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