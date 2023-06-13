#include "random_note_player.h"

#include <stdbool.h>
#include <math.h>
#include "raylib.h"
#include "util.h"
#include "sound_pool.h"

#define OCTAVES 3
#define NOTES_IN_OCTAVE 12

#define NOTES_LENGTH NOTES_IN_OCTAVE * OCTAVES + 1
#define MODES_LENGTH 4
#define MODE_NOTES_LENGTH 22

void RandomNotePlayer_PlayNote(Note note);
void RandomNotePlayer_InitNotes(SoundPool octavesSoundPools[]);
void RandomNotePlayer_InitModes(void);

typedef enum {
    NOTE_DO_4 = 0,
    NOTE_DO_SHARP_4,
    NOTE_RE_4,
    NOTE_RE_SHARP_4,
    NOTE_MI_4,
    NOTE_FA_4,
    NOTE_FA_SHARP_4,
    NOTE_SOL_4,
    NOTE_SOL_SHARP_4,
    NOTE_LA_4,
    NOTE_LA_SHARP_4,
    NOTE_SI_4,

    NOTE_DO_5,
    NOTE_DO_SHARP_5,
    NOTE_RE_5,
    NOTE_RE_SHARP_5,
    NOTE_MI_5,
    NOTE_FA_5,
    NOTE_FA_SHARP_5,
    NOTE_SOL_5,
    NOTE_SOL_SHARP_5,
    NOTE_LA_5,
    NOTE_LA_SHARP_5,
    NOTE_SI_5,

    NOTE_DO_6,
    NOTE_DO_SHARP_6,
    NOTE_RE_6,
    NOTE_RE_SHARP_6,
    NOTE_MI_6,
    NOTE_FA_6,
    NOTE_FA_SHARP_6,
    NOTE_SOL_6,
    NOTE_SOL_SHARP_6,
    NOTE_LA_6,
    NOTE_LA_SHARP_6,
    NOTE_SI_6,
    
    NOTE_DO_7
} Notes;

Note notes[NOTES_LENGTH];
Note modes[MODES_LENGTH][MODE_NOTES_LENGTH];

Note* currentMode;
float previousNote;

void RandomNotePlayer_Init(SoundPool octavesSoundPools[]) {
    RandomNotePlayer_InitNotes(octavesSoundPools);
    RandomNotePlayer_InitModes();
}

void RandomNotePlayer_Reset() {
    RandomNotePlayer_RandomizeMode();
    previousNote = NOTE_DO_4;
}

Note RandomNotePlayer_GenerateNote(int level, int maxLevel) {
    float currentNote = previousNote;
    bool playHardcodedNotes = level > 36;
    float min = easeInCirc((float) level / (float) maxLevel) * (MODE_NOTES_LENGTH - 1);
    float max = easeOutCirc((float) level / (float) maxLevel) * (MODE_NOTES_LENGTH - 1);
    float adjustedNote = GetRandomValueD(min, max);
    if (playHardcodedNotes) {
        currentNote = MODE_NOTES_LENGTH + (level - maxLevel - 1);
    } else if (GetRandomValue(0, 4) > 1) {
        currentNote += 2.0f;
        if (currentNote > max) {
            currentNote = (adjustedNote + max) / 2;
        }
    } else {
        currentNote -= 1.0f;
        if (currentNote < min) {
            currentNote = (adjustedNote + min) / 2;
        }
    }

    Note nextNote = currentMode[(int) round(currentNote)];
    previousNote = currentNote;
    return nextNote;
}

void RandomNotePlayer_RandomizeMode() {
    Note* previousMode = currentMode;
    do {
        currentMode = (Note*) &modes[GetRandomValue(0, MODES_LENGTH - 1)];
    } while (currentMode == previousMode);
}

inline float calculatePitch(int note) {
    return pow(2, ((float) note / (float) 12));
}

void RandomNotePlayer_InitNotes(SoundPool octavesSoundPools[]) {
    int notesArrayIndex = 0;
    for (int octaveIndex = 0 ; octaveIndex < OCTAVES ; octaveIndex++) {
        for (int noteIndex = 0 ; noteIndex < NOTES_IN_OCTAVE ; noteIndex++) {
            notes[notesArrayIndex] = (Note) { 
                .soundPool = &octavesSoundPools[octaveIndex], 
                .pitch = calculatePitch(noteIndex) 
            };
            notesArrayIndex++;
        }
    }
    // Do7, 2.0x of Do6
    notes[notesArrayIndex] = (Note) { .soundPool = &octavesSoundPools[OCTAVES - 1], .pitch = 2.0f };
}

void RandomNotePlayer_InitModes() {
    enum {
        MODE_IONIAN = 0,
        MODE_DORIAN,
        MODE_MIXOLYDIAN,
        MODE_LOCRIAN
    };
    
    // Ionian
    int noteIndex = 0;
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_DO_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_RE_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_MI_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_FA_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SOL_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_LA_4];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SI_4];
    
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_DO_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_RE_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_MI_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_FA_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SOL_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_LA_5];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SI_5];

    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_DO_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_RE_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_MI_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_FA_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SOL_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_LA_6];
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_SI_6];
    
    modes[MODE_IONIAN][noteIndex++] = notes[NOTE_DO_7]; // noteIndex = 21

    // Dorian
    noteIndex = 0;
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_DO_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_SHARP_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_FA_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_SOL_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_4];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_SHARP_4];
    
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_DO_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_SHARP_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_FA_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_SOL_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_5];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_SHARP_5];

    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_DO_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_RE_SHARP_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_FA_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_SOL_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_6];
    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_LA_SHARP_6];

    modes[MODE_DORIAN][noteIndex++] = notes[NOTE_DO_7]; // noteIndex = 21

    // Mixolydian
    noteIndex = 0;
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_DO_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_RE_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_MI_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_FA_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_SOL_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_4];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_SHARP_4];

    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_DO_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_RE_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_MI_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_FA_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_SOL_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_5];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_SHARP_5];

    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_DO_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_RE_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_MI_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_FA_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_SOL_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_6];
    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_LA_SHARP_6];

    modes[MODE_MIXOLYDIAN][noteIndex++] = notes[NOTE_DO_7]; // noteIndex = 21

    // Locrian
    noteIndex = 0;
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_SHARP_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_RE_SHARP_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_SHARP_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_SOL_SHARP_4];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_LA_SHARP_4];

    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_SHARP_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_RE_SHARP_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_SHARP_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_SOL_SHARP_5];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_LA_SHARP_5];
        
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_SHARP_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_RE_SHARP_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_FA_SHARP_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_SOL_SHARP_6];
    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_LA_SHARP_6];

    modes[MODE_LOCRIAN][noteIndex++] = notes[NOTE_DO_7]; // noteIndex = 21
}