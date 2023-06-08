#pragma once

#include "raylib.h"
#include "thread_list.h"

#define SOUNDPOOL_LENGTH 8

// Used to play the same sound in many "channels" 
// simultaneously. The max number of sounds playeing
// at the same time is SOUNDPOOL_LENGTH
typedef struct {
    Sound sounds[SOUNDPOOL_LENGTH];
    int lastPlayed;
} SoundPool;

void SoundPool_LoadSoundFromWave(SoundPool* self, Wave wave, ThreadList* threads);

void SoundPool_UnloadSound(SoundPool* soundPool);

void SoundPool_PlaySound(SoundPool* soundPool, float pitch);