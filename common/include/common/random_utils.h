#ifndef OS_IHW_RANDOM_UTILS_H
#define OS_IHW_RANDOM_UTILS_H

void rng_init_for_process(int programmer_id);

/* целое 0..99 */
int rng_0_99(void);

/* 0/1 с p=0.5 */
int rng_coin_50(void);

/* 1 с p=0.7, 0 с p=0.3 */
int rng_event_p70(void);

#endif // OS_IHW_RANDOM_UTILS_H
