#include "tests.h"

#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include "util.h"

void Tests_NoteProgression(const char* out, int maxLevel) {
    #define MODE_NOTES_LENGTH 22
    
    float previousNote;

    int lines = 40;
    int columns = 300;
    int result[lines][columns];
    for (int j = 0 ; j < columns ; j++) {
        previousNote = 0;
        for (int i = 0 ; i < lines ; i++) {
            float playNote = previousNote;
            float min = easeInCirc((float) i / (float) maxLevel) * (MODE_NOTES_LENGTH - 1);
            float max = easeOutCirc((float) i / (float) maxLevel) * (MODE_NOTES_LENGTH - 1);
            float mean = GetRandomValueD(min, max);
            if (i > 36) {
                playNote = MODE_NOTES_LENGTH + (i - maxLevel - 2);
            } else if (GetRandomValue(0, 3) > 1) {
                playNote += 2.0f;
                if (playNote > max) {
                    playNote -= 4.0f;
                    if (playNote < min) {
                        playNote = mean;
                    }
                }
            } else {
                playNote -= 1.0f;
                if (playNote < min) {
                    playNote += 2.0f;
                    if (playNote > max) {
                        playNote = mean;
                    }
                }
            }
            result[i][j] = (int) round(playNote);
            previousNote = playNote;
        }
    }

    FILE* file = fopen(out, "w");
    if (file != NULL) {
        for (int i = 0 ; i < lines ; i++) {
            fprintf(file, "%d ", i + 1);
            for (int j = 0 ; j < columns ; j++) {
                fprintf(file, "%d ", result[i][j]);
            }
            fprintf(file,"\n");
        }
    }
    fclose(file);
}

void Tests_AppendFile(const char* out, const char* text) {
    FILE* file = fopen(out, "a");
    if (file != NULL) {
        fprintf(file, "%s", text);
    }
    fclose(file);
}