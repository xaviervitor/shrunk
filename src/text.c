#include "text.h"

#include "util.h"

void Text_InitText(Text* text, Dimensions* screenDimensions, Font* font, const char* str, Alignment alignment, Color color) {
    text->screenDimensions = screenDimensions;
    text->font = font;
    text->text = str;
    text->alignment = alignment;
    text->color = color;
    Text_Update(text);
}

void Text_Update(Text* text) {
    text->size = MeasureTextEx(*(text->font), text->text, text->font->baseSize, 0);
    if (text->alignment == ALIGN_CENTER) {
        text->position = (Vector2) {
            .x = text->screenDimensions->width / 2 - text->size.x / 2, 
            .y = text->screenDimensions->height / 2 - text->size.y / 2
        };
    } else if (text->alignment == ALIGN_CENTER_BOTTOM) {
        text->position = (Vector2) { 
            .x = text->screenDimensions->width / 2 - text->size.x / 2, 
            .y = text->screenDimensions->height - text->size.y
        };
    }
}

void Text_DrawText(Text text) {
    DrawTextEx(*(text.font), text.text, text.position, text.font->baseSize, 0, text.color);
}