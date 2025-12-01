#include "perf.h"

/* Low-level CSR access helpers */
static inline void write_csr(const char *csr, uint32_t val)
{
    // You can’t put a string in inline asm, so make one helper per CSR instead.
    (void)csr; (void)val; // just here so compiler doesn't whine if unused
}

/* One reader per CSR (this pattern works with GCC for RISC-V) */
static inline uint32_t read_mcycle(void) {
    uint32_t v;
    asm volatile ("csrr %0, mcycle" : "=r"(v));
    return v;
}
static inline uint32_t read_minstret(void) {
    uint32_t v;
    asm volatile ("csrr %0, minstret" : "=r"(v));
    return v;
}
static inline uint32_t read_mhpmcounter3(void) {
    uint32_t v;
    asm volatile ("csrr %0, mhpmcounter3" : "=r"(v));
    return v;
}
/* same pattern for 4..9 */
static inline uint32_t read_mhpmcounter4(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter4" : "=r"(v)); return v; }
static inline uint32_t read_mhpmcounter5(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter5" : "=r"(v)); return v; }
static inline uint32_t read_mhpmcounter6(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter6" : "=r"(v)); return v; }
static inline uint32_t read_mhpmcounter7(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter7" : "=r"(v)); return v; }
static inline uint32_t read_mhpmcounter8(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter8" : "=r"(v)); return v; }
static inline uint32_t read_mhpmcounter9(void) { uint32_t v; asm volatile ("csrr %0, mhpmcounter9" : "=r"(v)); return v; }

/* Clear all counters to zero.
 * The DTEK-V handout shows the exact way to reset; often you can just write zero.
 * Adjust this according to that doc if needed.
 */
void clear_counters(void)
{
    asm volatile ("csrw mcycle,     zero");
    asm volatile ("csrw minstret,   zero");
    asm volatile ("csrw mhpmcounter3, zero");
    asm volatile ("csrw mhpmcounter4, zero");
    asm volatile ("csrw mhpmcounter5, zero");
    asm volatile ("csrw mhpmcounter6, zero");
    asm volatile ("csrw mhpmcounter7, zero");
    asm volatile ("csrw mhpmcounter8, zero");
    asm volatile ("csrw mhpmcounter9, zero");
}

void read_counters(PerfCounters *c)
{
    c->mcycle        = read_mcycle();
    c->minstret      = read_minstret();
    c->mhpm3_mem     = read_mhpmcounter3();
    c->mhpm4_ic_miss = read_mhpmcounter4();
    c->mhpm5_dc_miss = read_mhpmcounter5();
    c->mhpm6_ic_stall= read_mhpmcounter6();
    c->mhpm7_dc_stall= read_mhpmcounter7();
    c->mhpm8_dhaz_stall = read_mhpmcounter8();
    c->mhpm9_alu_stall  = read_mhpmcounter9();
}
