#ifndef PERF_H
#define PERF_H

#include <stdint.h>

typedef struct {
    uint32_t mcycle;       // # CPU cycles
    uint32_t minstret;     // # retired instructions
    uint32_t mhpm3_mem;    // # memory instructions
    uint32_t mhpm4_ic_miss;
    uint32_t mhpm5_dc_miss;
    uint32_t mhpm6_ic_stall;
    uint32_t mhpm7_dc_stall;
    uint32_t mhpm8_dhaz_stall;
    uint32_t mhpm9_alu_stall;
} PerfCounters;

void clear_counters(void);
void read_counters(PerfCounters *c);

#endif
