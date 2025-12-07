#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ten/config_10.h"

int observer_10_open_fifo(int obs_id);
void observer_10_run(int obs_id, int fifo_fd);

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <observer_id 0..%d>\n",
            prog, MAX_OBSERVERS_10 - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    const long obs_long = strtol(argv[1], &end, 10);
    if (*end != '\0' || obs_long < 0 || obs_long >= MAX_OBSERVERS_10) {
        usage(argv[0]);
        return 1;
    }
    const int obs_id = (int)obs_long;

    const int fd = observer_10_open_fifo(obs_id);
    if (fd < 0) {
        fprintf(stderr, "os_10_observer: failed to open fifo for observer %d\n",
                obs_id);
        return 1;
    }

    observer_10_run(obs_id, fd);

    close(fd);
    return 0;
}
