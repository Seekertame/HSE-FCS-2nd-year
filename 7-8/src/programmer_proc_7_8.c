#define POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "seven_eight/ipc_7_8.h"
#include "seven_eight/config_7_8.h"
#include "common/random_utils.h"
#include "common/log_utils.h"

extern volatile sig_atomic_t g_stop;

typedef enum {
    PSTATE_WRITING,
    PSTATE_WAITING_RESULT,
    PSTATE_REVIEWING,
    PSTATE_FIXING
} ProgrammerState;

static void simulate_work_short(void) {
    usleep(200 * 1000); // 200 ms
}

static int choose_reviewer(const int author_id) {
    const int first = (author_id + 1) % N_PROGRAMMERS;
    const int second = (author_id + 2) % N_PROGRAMMERS;
    return rng_coin_50() ? first : second;
}

static void send_program(const IpcContext7_8 *ctx,
                         const int author_id,
                         const int program_id,
                         const int version,
                         const int reviewer_id) {
    ProgramTask task;
    task.id = program_id;
    task.author_id = author_id;
    task.reviewer_id = reviewer_id;
    task.version = version;
    task.status = PROGRAM_STATUS_ON_REVIEW;
    task.result = PROGRAM_RESULT_NONE;

    ipc7_8_enqueue_task(ctx, reviewer_id, &task);
}

void run_programmer_7_8(const int id, const IpcContext7_8 *ctx) {
    rng_init_for_process(id);

    char msg[160];

    int own_program_id = 0;
    int own_version = 0;
    int reviewer_for_own = -1;
    int successes = 0;

    ProgrammerState st = PSTATE_WRITING;

    while (1) {
        if (g_stop) {
            break;
        }

        if (ipc7_8_reached_limit(ctx)) {
            snprintf(msg, sizeof(msg),
                     "global limit reached (TOTAL_ACCEPTED_LIMIT_7_8=%d), local OK=%d, exiting",
                     TOTAL_ACCEPTED_LIMIT_7_8, successes);
            log_event(id, msg);
            break;
        }

        switch (st) {
            case PSTATE_WRITING:
                snprintf(msg, sizeof(msg), "writing new program (OK_local=%d)", successes);
                log_event(id, msg);
                simulate_work_short();

                own_program_id++;
                own_version = 1;
                reviewer_for_own = choose_reviewer(id);
                send_program(ctx, id, own_program_id, own_version, reviewer_for_own);
                snprintf(msg, sizeof(msg), "sent program #%d v%d to P%d",
                         own_program_id, own_version, reviewer_for_own);
                log_event(id, msg);

                st = PSTATE_WAITING_RESULT;
                break;

            case PSTATE_WAITING_RESULT: {
                ProgramResult res;
                int pid, ver;
                const int got = ipc7_8_try_take_result(ctx, id, &res, &pid, &ver);
                if (got) {
                    if (res == PROGRAM_RESULT_OK) {
                        successes++;
                        const int total = ipc7_8_increment_total_accepted(ctx);

                        snprintf(msg, sizeof(msg),
                                 "got OK for program #%d v%d (OK_local=%d, OK_total=%d)",
                                 pid, ver, successes, total);
                        log_event(id, msg);

                        st = PSTATE_WRITING;
                    } else {
                        snprintf(msg, sizeof(msg),
                                 "got FAIL for program #%d v%d",
                                 pid, ver);
                        log_event(id, msg);
                        st = PSTATE_FIXING;
                    }
                    break;
                }

                if (ipc7_8_queue_has_items(ctx, id)) {
                    st = PSTATE_REVIEWING;
                    break;
                }

                log_event(id, "waiting for result (sleep)");
                simulate_work_short();
                break;
            }

            case PSTATE_REVIEWING: {
                ProgramTask task;
                const int has = ipc7_8_try_dequeue_task(ctx, id, &task);
                if (!has) {
                    st = PSTATE_WAITING_RESULT;
                    break;
                }

                snprintf(msg, sizeof(msg),
                         "reviewing program #%d v%d from P%d",
                         task.id, task.version, task.author_id);
                log_event(id, msg);

                simulate_work_short();

                const ProgramResult res =
                        rng_event_p70() ? PROGRAM_RESULT_OK : PROGRAM_RESULT_FAIL;

                ipc7_8_send_result(ctx, task.author_id,
                                   res, task.id, task.version);

                snprintf(msg, sizeof(msg),
                         "finished review of program #%d v%d from P%d: %s",
                         task.id, task.version, task.author_id,
                         res == PROGRAM_RESULT_OK ? "OK" : "FAIL");
                log_event(id, msg);

                st = PSTATE_WAITING_RESULT;
                break;
            }

            case PSTATE_FIXING:
                snprintf(msg, sizeof(msg),
                         "fixing own program #%d v%d",
                         own_program_id, own_version);
                log_event(id, msg);

                simulate_work_short();

                own_version++;
                send_program(ctx, id, own_program_id, own_version, reviewer_for_own);
                snprintf(msg, sizeof(msg),
                         "resubmitted program #%d v%d to P%d",
                         own_program_id, own_version, reviewer_for_own);
                log_event(id, msg);

                st = PSTATE_WAITING_RESULT;
                break;
        }
    }

    snprintf(msg, sizeof(msg),
             "stopping (7-8), local OK=%d", successes);
    log_event(id, msg);
}
