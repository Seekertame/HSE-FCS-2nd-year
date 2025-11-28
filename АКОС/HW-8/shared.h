#ifndef AKOS_HW_8_SHARED_H
#define AKOS_HW_8_SHARED_H

#include <semaphore.h>

#define SHM_NAME "/akos_hw8_shm"

#define BUFFER_SIZE 16

#define RANDOM_MIN 0
#define RANDOM_MAX 100

typedef struct {
    int terminate;

    size_t write_index;
    size_t read_index;
    int buffer[BUFFER_SIZE];

    sem_t mutex;
    sem_t slots;
    sem_t items;
} shared_data_t;

#endif
