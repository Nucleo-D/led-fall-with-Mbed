//
// Created by li-xinjia on 22/09/26.
//

#ifndef WS2812B_HPP
#define WS2812B_HPP

#if defined ( __GNUC__ )    /* GCC Compiler */
#define NOP __asm __volatile("nop\n")
#elif defined(__ICCARM__)     /* IAR */
#define NOP asm("NOP")
#else
#define NOP __nop()
#endif

#include <mbed.h>

class WS2812B {
private:
    unsigned int zero_high_times;
    unsigned int one_high_times;
    unsigned int zero_low_times;
    unsigned int one_low_times;
    int reset_us;
    uint8_t *trans_buffer;
    unsigned short max_port;
    DigitalOut pin;
public:
    WS2812B(PinName pin_name, unsigned short size,
            unsigned int zero_high_ns,
            unsigned int one_high_ns,
            unsigned int zero_low_ns,
            unsigned int one_low_ns,
            int reset_us) :
            reset_us(reset_us),
            pin(pin_name) {
        double freq = osKernelGetSysTimerFreq();
        double nano_second = 1000000000.0 / freq;
        zero_high_times = (zero_high_ns - 3 * nano_second) / (nano_second * 8);
        one_high_times = (one_high_ns - 3 * nano_second) / (nano_second * 8);
        zero_low_times = (zero_low_ns - 3 * nano_second) / (nano_second * 8);
        one_low_times = (one_low_ns - 3 * nano_second) / (nano_second * 8);


        max_port = size;
        trans_buffer = new uint8_t[size * 3]();
        pin = 0;
    }

    void setRGB(unsigned short port, uint8_t R, uint8_t G, uint8_t B) {
        if (port > max_port || port < 0) {
            return;
        }
        trans_buffer[port * 3] = G;
        trans_buffer[port * 3 + 1] = R;
        trans_buffer[port * 3 + 2] = B;
    }

    void update() {
        unsigned int j;
        reset();
        for (int i = 0; i < max_port * 3; ++i) {
            for (int k = 0; k < 8; ++k) {
                if ((trans_buffer[i] >> k) & 0x01) {
                    pin = 1;
                    for (j = 0; j < one_high_times; j++) {
                        NOP;
                    }
                    pin = 0;
                    for (j = 0; j < one_low_times; j++) {
                        NOP;
                    }
                } else {
                    pin = 1;
                    for (j = 0; j < zero_high_times; j++) {
                        NOP;
                    }
                    pin = 0;
                    for (j = 0; j < zero_low_times; j++) {
                        NOP;
                    }
                }
            }
        }
        pin = 1;
    }

    void reset() {
        pin = 0;
        wait_us(reset_us);
    }
};

#endif //WS2812B_HPP
