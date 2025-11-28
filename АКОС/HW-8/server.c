#define _POSIX_C_SOURCE 200809L

#include "shared_memory.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t running = 1;
static struct shared_data *shared = NULL;
static int shm_fd = -1;

static void request_shutdown(void) {
    running = 0;
    if (shared) {
        shared->terminate_flag = 1;
        if (shared->client_pid > 0) {
            kill(shared->client_pid, SIGTERM);
        }
    }
}

static void handle_signal(int sig) {
    (void)sig;
    request_shutdown();
}

static int setup_shared_memory(void) {
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return -1;
    }

    if (ftruncate(shm_fd, (off_t)sizeof(struct shared_data)) == -1) {
        perror("ftruncate");
        return -1;
    }

    shared = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        shared = NULL;
        return -1;
    }

    if (shared->client_pid == 0 && shared->server_pid == 0 && shared->has_value == 0 && shared->terminate_flag == 0) {
        memset(shared, 0, sizeof(struct shared_data));
    }

    if (shared->server_pid == 0) {
        shared->server_pid = getpid();
    }

    return 0;
}

static void cleanup(void) {
    if (shared) {
        if (shared->server_pid == getpid()) {
            shared->server_pid = 0;
        }
        shared->has_value = 0;
        shared->terminate_flag = 1;
        munmap(shared, sizeof(struct shared_data));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    shm_unlink(SHM_NAME);
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    if (setup_shared_memory() != 0) {
        return EXIT_FAILURE;
    }

    printf("Server started with PID %d. Press Ctrl+C to stop.\n", getpid());

    while (running) {
        if (shared->terminate_flag) {
            break;
        }

        if (shared->has_value) {
            printf("Received value: %d\n", shared->value);
            fflush(stdout);
            shared->has_value = 0;
        }

        struct timespec ts = {0, 100 * 1000 * 1000};
        nanosleep(&ts, NULL);
    }

    request_shutdown();
    cleanup();
    printf("Server stopped.\n");
    return EXIT_SUCCESS;
}
