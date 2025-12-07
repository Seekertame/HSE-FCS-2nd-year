#define POSIX_C_SOURCE 200809L

#include "seven_eight/ipc_7_8.h"
#include "seven_eight/config_7_8.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


static void build_name(char *buf, size_t sz, const char *prefix, const int idx) {
    snprintf(buf, sz, "%s%d", prefix, idx);
}

static int open_queue_semaphores(IpcContext7_8 *ctx, const int create) {
    char name[64];
    const int flags_create = O_CREAT | O_EXCL;

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        const int flags_open = 0;
        const int flags = create ? flags_create : flags_open;

        build_name(name, sizeof(name), SEM_NAME_QUEUE_MUTEX_PREFIX, i);
        ctx->queue_mutex[i] = sem_open(name, flags, 0666, 1);
        if (ctx->queue_mutex[i] == SEM_FAILED) {
            perror("sem_open queue_mutex");
            return -1;
        }

        build_name(name, sizeof(name), SEM_NAME_QUEUE_ITEMS_PREFIX, i);
        ctx->queue_items[i] = sem_open(name, flags, 0666, 0);
        if (ctx->queue_items[i] == SEM_FAILED) {
            perror("sem_open queue_items");
            return -1;
        }

        build_name(name, sizeof(name), SEM_NAME_QUEUE_SPACES_PREFIX, i);
        ctx->queue_spaces[i] = sem_open(name, flags, 0666, QUEUE_CAPACITY);
        if (ctx->queue_spaces[i] == SEM_FAILED) {
            perror("sem_open queue_spaces");
            return -1;
        }
    }
    return 0;
}

static int open_mailbox_semaphores(IpcContext7_8 *ctx, const int create) {
    char name[64];
    const int flags_create = O_CREAT | O_EXCL;

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        const int flags_open = 0;
        const int flags = create ? flags_create : flags_open;

        build_name(name, sizeof(name), SEM_NAME_MB_MUTEX_PREFIX, i);
        ctx->mb_mutex[i] = sem_open(name, flags, 0666, 1);
        if (ctx->mb_mutex[i] == SEM_FAILED) {
            perror("sem_open mb_mutex");
            return -1;
        }

        build_name(name, sizeof(name), SEM_NAME_MB_READY_PREFIX, i);
        ctx->mb_ready[i] = sem_open(name, flags, 0666, 0);
        if (ctx->mb_ready[i] == SEM_FAILED) {
            perror("sem_open mb_ready");
            return -1;
        }
    }
    return 0;
}

static int open_total_semaphore(IpcContext7_8 *ctx, int create) {
    const int flags_create = O_CREAT | O_EXCL;
    const int flags_open = 0;
    const int flags = create ? flags_create : flags_open;

    ctx->total_mutex = sem_open(SEM_NAME_TOTAL_MUTEX, flags, 0666, 1);
    if (ctx->total_mutex == SEM_FAILED) {
        perror("sem_open total_mutex");
        return -1;
    }
    return 0;
}

int ipc7_8_create_and_init(IpcContext7_8 *ctx, const int total_limit) {
    memset(ctx, 0, sizeof(*ctx));

    ctx->shm_fd = shm_open(SHM_NAME_7_8, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (ctx->shm_fd == -1) {
        perror("shm_open (create)");
        return -1;
    }

    if (ftruncate(ctx->shm_fd, sizeof(SharedState7_8)) == -1) {
        perror("ftruncate");
        close(ctx->shm_fd);
        shm_unlink(SHM_NAME_7_8);
        return -1;
    }

    void *addr = mmap(NULL, sizeof(SharedState7_8),
                      PROT_READ | PROT_WRITE, MAP_SHARED, ctx->shm_fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(ctx->shm_fd);
        shm_unlink(SHM_NAME_7_8);
        return -1;
    }

    ctx->state = (SharedState7_8 *) addr;

    memset(ctx->state, 0, sizeof(SharedState7_8));
    ctx->state->total_accepted = 0;
    ctx->state->total_limit = total_limit;

    if (open_queue_semaphores(ctx, 1) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        shm_unlink(SHM_NAME_7_8);
        return -1;
    }

    if (open_mailbox_semaphores(ctx, 1) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        shm_unlink(SHM_NAME_7_8);
        return -1;
    }

    if (open_total_semaphore(ctx, 1) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        shm_unlink(SHM_NAME_7_8);
        return -1;
    }

    return 0;
}

int ipc7_8_connect_existing(IpcContext7_8 *ctx) {
    memset(ctx, 0, sizeof(*ctx));

    ctx->shm_fd = shm_open(SHM_NAME_7_8, O_RDWR, 0666);
    if (ctx->shm_fd == -1) {
        perror("shm_open (open existing)");
        return -1;
    }

    void *addr = mmap(NULL, sizeof(SharedState7_8),
                      PROT_READ | PROT_WRITE, MAP_SHARED, ctx->shm_fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(ctx->shm_fd);
        return -1;
    }

    ctx->state = (SharedState7_8 *) addr;

    if (open_queue_semaphores(ctx, 0) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        return -1;
    }

    if (open_mailbox_semaphores(ctx, 0) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        return -1;
    }

    if (open_total_semaphore(ctx, 0) != 0) {
        munmap(ctx->state, sizeof(SharedState7_8));
        close(ctx->shm_fd);
        return -1;
    }

    return 0;
}

void ipc7_8_close(IpcContext7_8 *ctx) {
    if (!ctx) return;

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        if (ctx->queue_mutex[i]) sem_close(ctx->queue_mutex[i]);
        if (ctx->queue_items[i]) sem_close(ctx->queue_items[i]);
        if (ctx->queue_spaces[i]) sem_close(ctx->queue_spaces[i]);

        if (ctx->mb_mutex[i]) sem_close(ctx->mb_mutex[i]);
        if (ctx->mb_ready[i]) sem_close(ctx->mb_ready[i]);
    }

    if (ctx->total_mutex) sem_close(ctx->total_mutex);

    if (ctx->state) {
        munmap(ctx->state, sizeof(SharedState7_8));
        ctx->state = NULL;
    }

    if (ctx->shm_fd != -1) {
        close(ctx->shm_fd);
        ctx->shm_fd = -1;
    }
}

void ipc7_8_unlink_all(void) {
    char name[64];

    shm_unlink(SHM_NAME_7_8);

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        build_name(name, sizeof(name), SEM_NAME_QUEUE_MUTEX_PREFIX, i);
        sem_unlink(name);
        build_name(name, sizeof(name), SEM_NAME_QUEUE_ITEMS_PREFIX, i);
        sem_unlink(name);
        build_name(name, sizeof(name), SEM_NAME_QUEUE_SPACES_PREFIX, i);
        sem_unlink(name);

        build_name(name, sizeof(name), SEM_NAME_MB_MUTEX_PREFIX, i);
        sem_unlink(name);
        build_name(name, sizeof(name), SEM_NAME_MB_READY_PREFIX, i);
        sem_unlink(name);
    }

    sem_unlink(SEM_NAME_TOTAL_MUTEX);
}

void ipc7_8_enqueue_task(const IpcContext7_8 *ctx, const int reviewer_id,
                         const ProgramTask *task) {
    ReviewQueue7_8 *q = &ctx->state->queues[reviewer_id];

    sem_wait(ctx->queue_spaces[reviewer_id]);
    sem_wait(ctx->queue_mutex[reviewer_id]);

    q->buffer[q->tail] = *task;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;

    sem_post(ctx->queue_mutex[reviewer_id]);
    sem_post(ctx->queue_items[reviewer_id]);
}

int ipc7_8_try_dequeue_task(const IpcContext7_8 *ctx, const int reviewer_id,
                            ProgramTask *out_task) {
    ReviewQueue7_8 *q = &ctx->state->queues[reviewer_id];

    if (sem_trywait(ctx->queue_items[reviewer_id]) == -1) {
        if (errno == EAGAIN) {
            return 0;
        }
        perror("sem_trywait queue_items");
        return 0;
    }

    sem_wait(ctx->queue_mutex[reviewer_id]);

    *out_task = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;

    sem_post(ctx->queue_mutex[reviewer_id]);
    sem_post(ctx->queue_spaces[reviewer_id]);
    return 1;
}

int ipc7_8_queue_has_items(const IpcContext7_8 *ctx, const int reviewer_id) {
    int value = 0;
    if (sem_getvalue(ctx->queue_items[reviewer_id], &value) == -1) {
        perror("sem_getvalue queue_items");
        return 0;
    }
    return value > 0;
}

void ipc7_8_send_result(const IpcContext7_8 *ctx, const int author_id,
                        const ProgramResult res, const int program_id, const int version) {
    ResultMailbox7_8 *mb = &ctx->state->mailboxes[author_id];

    sem_wait(ctx->mb_mutex[author_id]);

    mb->result = res;
    mb->program_id = program_id;
    mb->version = version;
    mb->has_result = 1;

    sem_post(ctx->mb_mutex[author_id]);
    sem_post(ctx->mb_ready[author_id]);
}

int ipc7_8_try_take_result(const IpcContext7_8 *ctx, const int author_id,
                           ProgramResult *out_res,
                           int *out_program_id,
                           int *out_version) {
    ResultMailbox7_8 *mb = &ctx->state->mailboxes[author_id];

    if (sem_trywait(ctx->mb_ready[author_id]) == -1) {
        if (errno == EAGAIN) {
            return 0;
        }
        perror("sem_trywait mb_ready");
        return 0;
    }

    sem_wait(ctx->mb_mutex[author_id]);

    *out_res = mb->result;
    *out_program_id = mb->program_id;
    *out_version = mb->version;
    mb->has_result = 0;

    sem_post(ctx->mb_mutex[author_id]);
    return 1;
}

int ipc7_8_increment_total_accepted(const IpcContext7_8 *ctx) {
    sem_wait(ctx->total_mutex);

    if (ctx->state->total_limit <= 0) {
        ctx->state->total_accepted++;
    } else if (ctx->state->total_accepted < ctx->state->total_limit) {
        ctx->state->total_accepted++;
    }
    const int value = ctx->state->total_accepted;

    sem_post(ctx->total_mutex);
    return value;
}

int ipc7_8_reached_limit(const IpcContext7_8 *ctx) {
    sem_wait(ctx->total_mutex);
    const int accepted = ctx->state->total_accepted;
    const int limit = ctx->state->total_limit;
    sem_post(ctx->total_mutex);

    if (limit <= 0) return 0;
    return accepted >= limit;
}
