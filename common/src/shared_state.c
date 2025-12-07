#define POSIX_C_SOURCE 200809L

#include "common/shared_state.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>

void shared_state_init(SharedState *st) {
    memset(st, 0, sizeof(*st));

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        ReviewQueue *q = &st->queues[i];
        q->head = q->tail = q->count = 0;
        sem_init(&q->mutex, 1, 1);
        sem_init(&q->items, 1, 0);
        sem_init(&q->spaces, 1, QUEUE_CAPACITY);

        ResultMailbox *mb = &st->mailboxes[i];
        mb->result = PROGRAM_RESULT_NONE;
        mb->program_id = -1;
        mb->version = 0;
        mb->has_result = 0;
        sem_init(&mb->mutex, 1, 1);
        sem_init(&mb->ready, 1, 0);
    }

    st->total_accepted = 0;
    st->total_limit = 0;
    sem_init(&st->total_mutex, 1, 1);
}

void shared_state_destroy(SharedState *st) {
    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        ReviewQueue *q = &st->queues[i];
        sem_destroy(&q->mutex);
        sem_destroy(&q->items);
        sem_destroy(&q->spaces);

        ResultMailbox *mb = &st->mailboxes[i];
        sem_destroy(&mb->mutex);
        sem_destroy(&mb->ready);
    }
    sem_destroy(&st->total_mutex);
}

void shared_state_enqueue_task(SharedState *st, int reviewer_id, const ProgramTask *task) {
    ReviewQueue *q = &st->queues[reviewer_id];

    sem_wait(&q->spaces);
    sem_wait(&q->mutex);

    q->buffer[q->tail] = *task;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;

    sem_post(&q->mutex);
    sem_post(&q->items);
}

int shared_state_try_dequeue_task(SharedState *st, int reviewer_id, ProgramTask *out_task) {
    ReviewQueue *q = &st->queues[reviewer_id];

    if (sem_trywait(&q->items) == -1) {
        if (errno == EAGAIN) {
            return 0;
        }
        perror("sem_trywait(items)");
        return 0;
    }

    sem_wait(&q->mutex);

    *out_task = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;

    sem_post(&q->mutex);
    sem_post(&q->spaces);
    return 1;
}

int shared_state_queue_has_items(SharedState *st, int reviewer_id) {
    ReviewQueue *q = &st->queues[reviewer_id];
    int value = 0;
    if (sem_getvalue(&q->items, &value) == -1) {
        perror("sem_getvalue(items)");
        return 0;
    }
    return value > 0;
}

void shared_state_send_result(SharedState *st, const int author_id,
                              const ProgramResult res, const int program_id, const int version) {
    ResultMailbox *mb = &st->mailboxes[author_id];

    sem_wait(&mb->mutex);

    mb->result = res;
    mb->program_id = program_id;
    mb->version = version;
    mb->has_result = 1;

    sem_post(&mb->mutex);
    sem_post(&mb->ready);
}

int shared_state_try_take_result(SharedState *st, const int author_id,
                                 ProgramResult *out_res,
                                 int *out_program_id,
                                 int *out_version) {
    ResultMailbox *mb = &st->mailboxes[author_id];

    if (sem_trywait(&mb->ready) == -1) {
        if (errno == EAGAIN) {
            return 0;
        }
        perror("sem_trywait(ready)");
        return 0;
    }

    sem_wait(&mb->mutex);

    *out_res = mb->result;
    *out_program_id = mb->program_id;
    *out_version = mb->version;
    mb->has_result = 0;

    sem_post(&mb->mutex);
    return 1;
}


void shared_state_set_total_limit(SharedState *st, const int limit) {
    sem_wait(&st->total_mutex);
    st->total_limit = limit;
    st->total_accepted = 0;
    sem_post(&st->total_mutex);
}

int shared_state_increment_total_accepted(SharedState *st) {
    sem_wait(&st->total_mutex);
    if (st->total_limit <= 0) {
        st->total_accepted++;
    } else {
        if (st->total_accepted < st->total_limit) {
            st->total_accepted++;
        }
    }
    const int value = st->total_accepted;
    sem_post(&st->total_mutex);
    return value;
}

int shared_state_get_total_accepted(SharedState *st) {
    sem_wait(&st->total_mutex);
    const int value = st->total_accepted;
    sem_post(&st->total_mutex);
    return value;
}

int shared_state_reached_limit(SharedState *st) {
    sem_wait(&st->total_mutex);
    const int accepted = st->total_accepted;
    const int limit = st->total_limit;
    sem_post(&st->total_mutex);

    if (limit <= 0) {
        return 0;
    }
    return accepted >= limit;
}
