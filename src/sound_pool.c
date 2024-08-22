#include "sound_pool.h"

#include <pthread.h>
#include <stdlib.h>
#include "raylib.h"
#include "thread_list.h"

void* loadSoundThread(void* _threadInfo);

typedef struct ThreadInfo {
    SoundPool* soundPool;
    Wave wave;
    int index;
} ThreadInfo;

void* loadSoundThread(void* _threadInfo) {
    ThreadInfo* threadInfo = (ThreadInfo*) _threadInfo;
    Sound sound = LoadSoundFromWave(threadInfo->wave);
    threadInfo->soundPool->sounds[threadInfo->index] = sound;
    free(threadInfo);
    return NULL;
}

void SoundPool_LoadSoundFromWave(SoundPool* self, Wave wave, ThreadList* threads) {
    self->lastPlayed = -1;
    for (int i = 0 ; i < SOUNDPOOL_LENGTH ; i++) {
        ThreadInfo* threadInfo = (ThreadInfo*) malloc(sizeof(ThreadInfo));
        threadInfo->soundPool = self;
        threadInfo->wave = wave;
        threadInfo->index = i;
        pthread_t thread;
        pthread_create(&thread, NULL, loadSoundThread, threadInfo);
        ThreadList_Add(threads, thread);
    }
}

void SoundPool_UnloadSound(SoundPool* self) {
    for (int i = 0 ; i < SOUNDPOOL_LENGTH ; i++) {
        Sound sound = self->sounds[i];
        UnloadSound(sound);
    }
}

void SoundPool_PlaySound(SoundPool* self, float pitch) {
    self->lastPlayed++;
    if (self->lastPlayed > SOUNDPOOL_LENGTH - 1) {
        self->lastPlayed = 0;
    }

    Sound sound = self->sounds[self->lastPlayed];
    SetSoundPitch(sound, pitch);
    PlaySound(sound);
}