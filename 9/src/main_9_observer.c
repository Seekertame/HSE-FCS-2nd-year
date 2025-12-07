#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>

#include "nine/config_9.h"

int observer_9_open_fifo(void);
void observer_9_run(int fifo_fd);

int main(void) {
    const int fd = observer_9_open_fifo();
    if (fd < 0) {
        fprintf(stderr, "os_9_observer: failed to open fifo '%s'\n",
                OBSERVER_FIFO_PATH_9);
        return 1;
    }

    observer_9_run(fd);

    close(fd);
    return 0;
}
