#pragma once

#include "sound_pool.h"

typedef struct Note {
    SoundPool* soundPool;
    float pitch;
} Note;

void RandomNotePlayer_Init(SoundPool octavesSoundPools[]);

void RandomNotePlayer_Reset();

// Plays a random note based in the note played previously.
// The current note can be 2 tones above or 1 tone below
// the previous note. If this new note is out of the min
// and max functions, a new note in bounds is selected.
// Above a certain level, the notes are hardcoded as the
// last notes in the current mode (See RandomNotePlayer_initModes()).
Note RandomNotePlayer_GenerateNote(int level, int maxLevel);

void RandomNotePlayer_RandomizeMode();

// Loads the notes[] array with (Note)s in order:
// "Do = 0, Do#, Re, Re#, Mi, Fa, Fa#, Sol, Sol#, La, La#, Si"
// in different octaves. In total, there's 37 notes
// going from Do4 to Do7, represented by a soundpool cointaining
// a loaded .wav tone (Do4, Do5, Do6) and a pitch.
void RandomNotePlayer_InitNotes(SoundPool octavesSoundPools[]);

// Initializes 4 scales of the modes Ionian, Dorian, Mixolydian and
// Locrian into the modes matrix, that can be accessed as modes[mode][note].
// Each array of notes has 22 notes, traversing the scale in the indicated
// mode sequence going from Do4 to Do7.
void RandomNotePlayer_InitModes();