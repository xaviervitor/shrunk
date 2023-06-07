#pragma once

#include <pthread.h>

// Dynamic array of threads
typedef struct ThreadList {
    size_t size;
    size_t maxSize;
    pthread_t* array;
} ThreadList;

ThreadList* ThreadList_New();

void ThreadList_Destroy(ThreadList* self);

void ThreadList_Add(ThreadList* self, pthread_t value);

void ThreadList_Join(ThreadList* self);