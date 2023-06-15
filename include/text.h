#pragma once

#include "raylib.h"
#include "util.h"

typedef enum {
    ALIGN_CENTER,
    ALIGN_CENTER_BOTTOM
} Alignment;

typedef struct Text {
    Font* font;
    const char* text;
    Vector2 position;
    Alignment alignment;
    Vector2 size;
    Color color;
} Text;


void Text_InitText(Text* text, Font* font, const char* str, Alignment alignment, Color color);

void Text_Update(Text* text);

void Text_DrawText(Text text);