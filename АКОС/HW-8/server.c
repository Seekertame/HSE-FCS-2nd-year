#include "shared.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static volatile sig_atomic_t g_stop = 0;
static shared_data_t *g_shared = NULL;

static void handle_signal(int signo) {
    (void) signo;
    g_stop = 1;
    if (g_shared != NULL) {
        g_shared->terminate = 1;
        (void) sem_post(&g_shared->items);
        (void) sem_post(&g_shared->slots);
    }
}

static void setup_signal_handlers(void) {
    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM)");
        exit(EXIT_FAILURE);
    }
}

static shared_data_t *map_shared_memory(int *out_fd, int *out_is_creator) {
    int is_creator = 0;

    int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd >= 0) {
        is_creator = 1;
        if (ftruncate(fd, (off_t) sizeof(shared_data_t)) == -1) {
            perror("ftruncate");
            close(fd);
            exit(EXIT_FAILURE);
        }
    } else if (errno == EEXIST) {
        fd = shm_open(SHM_NAME, O_RDWR, 0);
        if (fd == -1) {
            perror("shm_open(existing)");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    shared_data_t *data = mmap(NULL, sizeof(shared_data_t),
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (is_creator) {
        memset(data, 0, sizeof(*data));

        if (sem_init(&data->mutex, 1, 1) == -1) {
            perror("sem_init(mutex)");
            exit(EXIT_FAILURE);
        }
        if (sem_init(&data->slots, 1, BUFFER_SIZE) == -1) {
            perror("sem_init(slots)");
            exit(EXIT_FAILURE);
        }
        if (sem_init(&data->items, 1, 0) == -1) {
            perror("sem_init(items)");
            exit(EXIT_FAILURE);
        }

        data->terminate = 0;
        data->write_index = 0;
        data->read_index = 0;
    }

    *out_fd = fd;
    *out_is_creator = is_creator;
    return data;
}

int main(void) {
    int fd = -1;
    int is_creator = 0;

    shared_data_t *data = map_shared_memory(&fd, &is_creator);
    g_shared = data;
    setup_signal_handlers();

    printf("Server started. Press Ctrl+C in ANY of the processes to stop.\n");

    while (!g_stop && !data->terminate) {
        if (sem_wait(&data->items) == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("sem_wait(items)");
            break;
        }

        if (g_stop || data->terminate) {
            (void) sem_post(&data->items);
            break;
        }

        if (sem_wait(&data->mutex) == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("sem_wait(mutex)");
            break;
        }

        int value = data->buffer[data->read_index];
        data->read_index = (data->read_index + 1) % BUFFER_SIZE;

        if (sem_post(&data->mutex) == -1) {
            perror("sem_post(mutex)");
            break;
        }

        if (sem_post(&data->slots) == -1) {
            perror("sem_post(slots)");
            break;
        }

        printf("Server: received %d\n", value);
        fflush(stdout);
    }

    printf("Server: shutting down\n");

    if (is_creator) {
        if (sem_destroy(&data->mutex) == -1) {
            perror("sem_destroy(mutex)");
        }
        if (sem_destroy(&data->slots) == -1) {
            perror("sem_destroy(slots)");
        }
        if (sem_destroy(&data->items) == -1) {
            perror("sem_destroy(items)");
        }
    }

    if (munmap(data, sizeof(*data)) == -1) {
        perror("munmap");
    }

    if (close(fd) == -1) {
        perror("close");
    }

    if (is_creator) {
        if (shm_unlink(SHM_NAME) == -1 && errno != ENOENT) {
            perror("shm_unlink");
        }
    }

    return 0;
}
