#include <stdint.h>
#include "dtekv-lib.h"

#define LED_ADDRESS 0x04000000u

static inline void set_leds(uint32_t mask) {
    mask &= 0x3FFu;
    *(volatile uint32_t *)LED_ADDRESS = mask;
}

/* Stub for external IRQs – required by boot.S */
void handle_interrupt(unsigned cause) {
    (void)cause;
    // no interrupt handling in this minimal test
}

int main(void) {
    print(">>> HELLO from minimal main()\n");

    uint32_t v = 1;

    while (1) {
        set_leds(v);
        // crude delay
        for (volatile uint32_t i = 0; i < 500000; ++i) { }
        v ^= 1; // toggle LED0
    }

    return 0;
}
