#define POSIX_C_SOURCE 200809L

#include "common/log_utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#define MAX_OBSERVER_FDS 8

static double g_start_ms = -1.0;
static int g_observer_fds[MAX_OBSERVER_FDS];
static int g_observer_count = 0;

double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const double t = ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
    if (g_start_ms < 0.0) {
        g_start_ms = t;
    }
    return t - g_start_ms;
}

void log_clear_observers(void) {
    g_observer_count = 0;
}

void log_add_observer_fd(const int fd) {
    if (fd < 0) return;
    if (g_observer_count >= MAX_OBSERVER_FDS) return;
    g_observer_fds[g_observer_count++] = fd;
}

void log_set_observer_fd(const int fd) {
    log_clear_observers();
    log_add_observer_fd(fd);
}

void log_event(const int who, const char *fmt, ...) {
    char msg[256];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);

    const double t = now_ms();

    char tag_buf[16];
    const char *tag;
    if (who < 0) {
        tag = "SYS";
    } else {
        snprintf(tag_buf, sizeof tag_buf, "P%d", who);
        tag = tag_buf;
    }

    char line[512];
    int len = snprintf(line, sizeof line, "[%.3f][%s] %s\n", t, tag, msg);
    if (len <= 0) {
        return;
    }
    if (len > (int) sizeof line) {
        len = (int) sizeof line;
    }

    fwrite(line, 1, len, stdout);
    fflush(stdout);

    for (int i = 0; i < g_observer_count; ++i) {
        const int fd = g_observer_fds[i];
        if (fd >= 0) {
            (void) write(fd, line, (size_t) len);
        }
    }
}
