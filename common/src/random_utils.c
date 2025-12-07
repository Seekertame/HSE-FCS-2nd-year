#include "common/random_utils.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static int rng_initialized = 0;

void rng_init_for_process(const int programmer_id) {
    const unsigned int seed =
            (unsigned int) (time(NULL) ^ (getpid() << 16) ^ (programmer_id * 12345));
    srand(seed);
    rng_initialized = 1;
}

static void ensure_rng(void) {
    if (!rng_initialized) {
        rng_init_for_process(0);
    }
}

int rng_0_99(void) {
    ensure_rng();
    return rand() % 100;
}

int rng_coin_50(void) {
    return rng_0_99() < 50 ? 1 : 0;
}

int rng_event_p70(void) {
    return rng_0_99() < 70 ? 1 : 0;
}
