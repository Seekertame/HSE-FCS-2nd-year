#ifndef OS_IHW_SHARED_STATE_H
#define OS_IHW_SHARED_STATE_H

#include <semaphore.h>
#include "common/config.h"
#include "common/model.h"

typedef struct {
    ProgramTask buffer[QUEUE_CAPACITY];
    int head;
    int tail;
    int count;

    sem_t mutex;
    sem_t items;
    sem_t spaces;
} ReviewQueue;

typedef struct {
    ProgramResult result;
    int program_id;
    int version;
    int has_result;

    sem_t mutex;
    sem_t ready;
} ResultMailbox;

typedef struct {
    ReviewQueue queues[N_PROGRAMMERS];
    ResultMailbox mailboxes[N_PROGRAMMERS];

    int total_accepted;
    int total_limit;
    sem_t total_mutex;
} SharedState;

void shared_state_init(SharedState *st);

void shared_state_destroy(SharedState *st);

void shared_state_enqueue_task(SharedState *st, int reviewer_id, const ProgramTask *task);

int shared_state_try_dequeue_task(SharedState *st, int reviewer_id, ProgramTask *out_task);

int shared_state_queue_has_items(SharedState *st, int reviewer_id);

void shared_state_send_result(SharedState *st, int author_id,
                              ProgramResult res, int program_id, int version);

int shared_state_try_take_result(SharedState *st, int author_id,
                                 ProgramResult *out_res,
                                 int *out_program_id,
                                 int *out_version);

void shared_state_set_total_limit(SharedState *st, int limit);

int shared_state_increment_total_accepted(SharedState *st);

int shared_state_get_total_accepted(SharedState *st);

int shared_state_reached_limit(SharedState *st);

#endif // OS_IHW_SHARED_STATE_H
