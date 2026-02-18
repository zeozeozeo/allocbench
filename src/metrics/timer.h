#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_start(void);
double timer_end_ms(void);
double timer_end_us(void);
double timer_end_ns(void);
double timer_get_elapsed_ms(void);
double timer_get_elapsed_us(void);
double timer_get_elapsed_ns(void);

typedef struct {
    uint64_t start_cycles;
    uint64_t end_cycles;
    int running;
} hr_timer_t;

void hr_timer_init(hr_timer_t* timer);
void hr_timer_start(hr_timer_t* timer);
double hr_timer_end(hr_timer_t* timer);
double hr_timer_elapsed(hr_timer_t* timer);

uint64_t get_cycles(void);
double cycles_to_ns(uint64_t cycles);
double cycles_to_us(uint64_t cycles);
double cycles_to_ms(uint64_t cycles);

#endif
