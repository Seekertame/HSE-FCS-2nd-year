#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include "seven_eight/ipc_7_8.h"
#include "seven_eight/config_7_8.h"
#include "common/log_utils.h"

int main(void) {
    ipc7_8_unlink_all();
    IpcContext7_8 ctx;

    if (ipc7_8_create_and_init(&ctx, TOTAL_ACCEPTED_LIMIT_7_8) != 0) {
        fprintf(stderr, "os_7_8_init: failed to create and init IPC\n");
        return 1;
    }

    log_event(-1, "7-8 init: shared memory and named semaphores created");
    ipc7_8_close(&ctx);
    return 0;
}
