#define POSIX_C_SOURCE 200809L

#include "seven_eight/ipc_7_8.h"

void run_programmer_7_8(int id, IpcContext7_8 *ctx);

void run_programmer_9(const int id, IpcContext7_8 *ctx) {
    run_programmer_7_8(id, ctx);
}
