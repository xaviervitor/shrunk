#include "thread_list.h"
#include <pthread.h>
#include <stdlib.h>

ThreadList* ThreadList_New() {
    ThreadList* self = malloc(sizeof(ThreadList));
    self->size = 0;
    self->maxSize = 1;
    self->array = malloc(self->maxSize * sizeof(pthread_t));
    return self;
}

void ThreadList_Destroy(ThreadList* self) {
    free(self->array);
    free(self);
}

void ThreadList_Add(ThreadList* self, pthread_t value) {
    if (self->size >= self->maxSize) {
        self->maxSize = self->maxSize * 2;
        self->array = realloc(self->array, self->maxSize * sizeof(pthread_t));
    }
    self->array[self->size] = value;
    self->size++;
}

void ThreadList_Join(ThreadList* self) {
    for (size_t i = 0 ; i < self->size ; i++) {
        pthread_join(self->array[i], NULL);
    }
}