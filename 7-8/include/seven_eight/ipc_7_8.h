#ifndef OS_IHW_IPC_7_8_H
#define OS_IHW_IPC_7_8_H

#include <semaphore.h>
#include "common/config.h"
#include "common/model.h"

typedef struct {
    ProgramTask buffer[QUEUE_CAPACITY];
    int head;
    int tail;
    int count;
} ReviewQueue7_8;

typedef struct {
    ProgramResult result;
    int program_id;
    int version;
    int has_result;
} ResultMailbox7_8;

typedef struct {
    ReviewQueue7_8 queues[N_PROGRAMMERS];
    ResultMailbox7_8 mailboxes[N_PROGRAMMERS];

    int total_accepted;
    int total_limit;
} SharedState7_8;

typedef struct {
    SharedState7_8 *state;
    int shm_fd;

    sem_t *queue_mutex[N_PROGRAMMERS];
    sem_t *queue_items[N_PROGRAMMERS];
    sem_t *queue_spaces[N_PROGRAMMERS];

    sem_t *mb_mutex[N_PROGRAMMERS];
    sem_t *mb_ready[N_PROGRAMMERS];

    sem_t *total_mutex;
} IpcContext7_8;


int ipc7_8_create_and_init(IpcContext7_8 *ctx, int total_limit);

int ipc7_8_connect_existing(IpcContext7_8 *ctx);

void ipc7_8_close(IpcContext7_8 *ctx);

void ipc7_8_unlink_all(void);


void ipc7_8_enqueue_task(const IpcContext7_8 *ctx, int reviewer_id,
                         const ProgramTask *task);

int ipc7_8_try_dequeue_task(const IpcContext7_8 *ctx, int reviewer_id,
                            ProgramTask *out_task);

int ipc7_8_queue_has_items(const IpcContext7_8 *ctx, int reviewer_id);

void ipc7_8_send_result(const IpcContext7_8 *ctx, int author_id,
                        ProgramResult res, int program_id, int version);

int ipc7_8_try_take_result(const IpcContext7_8 *ctx, int author_id,
                           ProgramResult *out_res,
                           int *out_program_id,
                           int *out_version);


int ipc7_8_increment_total_accepted(const IpcContext7_8 *ctx);

int ipc7_8_reached_limit(const IpcContext7_8 *ctx);

#endif // OS_IHW_IPC_7_8_H
