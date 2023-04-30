//This is just to be compiled with a lot of libs.
//This should never do anything great to your pico, don't run it!

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/regs/addressmap.h"
#include "hardware/resets.h"
#include "hardware/rtc.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "pico/unique_id.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "hardware/interp.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/i2c.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/rtc.h"
#include "hardware/structs/uart.h"

int main() {
	stdio_init_all();
	
	printf("Testing pico/stdlib.h: %d\n", time_us_32());
	
	gpio_init(2);
	gpio_set_dir(2, GPIO_OUT);
	gpio_put(2, 1);
	sleep_ms(500);
	gpio_put(2, 0);
	printf("Testing hardware/gpio.h: Blinking LED on pin 2\n");
	
	adc_init();
	printf("Testing hardware/adc.h: ADC value on pin 26: %d\n", adc_read());
	
	PIO pio = pio0;
	uint sm = 0;
	pio_sm_config c = pio_get_default_sm_config();
	sm_config_set_in_pins(&c, 2);
	sm_config_set_jmp_pin(&c, 3);
	sm_config_set_out_pins(&c, -1, 1);
	pio_sm_set_enabled(pio, sm, true);
	
	pwm_config config = pwm_get_default_config();
	pwm_set_gpio_level(2, 100);
	printf("Testing hardware/pwm.h: PWM output on pin 2\n");
	
	printf("Testing hardware/regs/addressmap.h: %x\n", IO_BANK0_BASE);
	
	printf("Testing hardware/resets.h: IOQSPI reset\n");
	
	printf("Testing hardware/rtc.h: RTC alarm set\n");
	
	spin_lock_t lock;

	printf("Testing hardware/sync.h: Spin lock acquired\n");
	printf("Testing hardware/sync.h: Spin lock released\n");

	uart_init(uart0, 115200);
	uart_puts(uart0, "Testing hardware/uart.h: Hello, UART!\n");

	uint32_t unique_id[4];
	printf("Testing pico/unique_id.h: Unique ID: %08X%08X%08X%08X\n", unique_id[3], unique_id[2], unique_id[1], unique_id[0]);

	printf("Testing hardware/flash.h: Flash range erase\n");

	irq_set_enabled(UART0_IRQ, true);
	printf("Testing hardware/irq.h: UART0 IRQ enabled\n");

	return 0;
}
