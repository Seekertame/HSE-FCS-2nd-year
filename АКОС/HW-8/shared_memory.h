#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>

#define SHM_NAME "/hw8_shm"
#define RANDOM_RANGE 100

struct shared_data {
    pid_t client_pid;
    pid_t server_pid;
    volatile sig_atomic_t terminate_flag;
    volatile sig_atomic_t has_value;
    int value;
};

#endif // SHARED_MEMORY_H
