#pragma once

#include "raylib.h"

#define RAYLIB_RAND_MAX 32767
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

typedef struct Dimensions {
    int width;
    int height;
} Dimensions;

typedef struct Circle {
    Texture2D texture;
    Vector2 position;
    float radius;
    Color color;
} Circle;

typedef struct ColorPalette {
    Color background;
    Color foreground;
} ColorPalette;

typedef struct Range {
    int min;
    int max;
} Range;

void drawCircle(Circle circle);

void toggleGameFullscreen(void);

void initRandom(void);

float GetRandomValueF(float min, float max);

double GetRandomValueD(double min, double max);

float easeInCirc(float x);

float easeOutCirc(float x);