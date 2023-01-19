#include <stdio.h>
#include <stdlib.h>

#include "Icons.hpp"
#include "ProotAnims.hpp"
#include "hardware/regs/rosc.h"
#include "hardware/sync.h"
#include "headers/Emotion.hpp"
#include "headers/Logger.hpp"
#include "headers/Max7219Driver.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning ProtoTracer Reality Coprocessor requires the builtin LED for status.
#endif

extern Logger myLogger;

Max7219Driver myDriver(1);

Emotion Normal("Normal", eye, nose, maw);
Emotion Suprise("Suprised", Spooked, nose, maw);
Emotion VwV("VwV", vwv, nose, maw);

Emotion *currentAnim = &Normal;

Animation blinkAnimation(blinkAnim, &currentAnim);

void seed_random_from_rosc();

uint8_t emote = 0;

bool emoteChange(repeating_timer_t *rt) {
	switch (emote) {
		case 0:
			currentAnim = &Normal;
			emote++;
			break;
		
		case 1:
			currentAnim = &Suprise;
			emote++;
			break;
		
		case 2:
			currentAnim = &VwV;
			emote = 0;
			break;

		default:
			emote = 0;
			break;
	}

	if (blinkStep == 0) {
		currentAnim->drawAll();
		myDriver.display();
	} else {
		currentAnim->drawMaw();
		currentAnim->drawNose();
	}

	return true;
}

int main() {
	seed_random_from_rosc();
	stdio_init_all();
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	// while (!tud_cdc_connected()) {  // Wait for USB to connect
	// 	sleep_ms(100);
	// 	gpio_xor_mask(1 << PICO_DEFAULT_LED_PIN);  // hehe toggle
	// }

	gpio_put(PICO_DEFAULT_LED_PIN, false);
	if (myDriver.initializeDisplays()) {
		myLogger.log("Displays successfully initiailized!\n");
	} else {
		myLogger.log("Displays failed to initialize!");
	}

	myLogger.log("Actual baudrate set: %u\n", spi_get_baudrate(spi0));

	currentAnim->drawAll();
	myDriver.display();

	repeating_timer_t changeTimer;
	add_repeating_timer_ms(-1000, emoteChange, 0, &changeTimer);

	while (true) {
		int nextBlink = (rand() & 0x0FFF) + 2000;  // This is an awful random number generator. But, who cares.
		myLogger.logDebug("setting next blink for %d ms in the future\n", nextBlink);
		blinkAnimation.startAnimation(nextBlink);
		sleep_ms(nextBlink);
	}
}

void seed_random_from_rosc() {
	uint32_t random = 0x811c9dc5;
	uint8_t next_byte = 0;
	volatile uint32_t *rnd_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

	for (int i = 0; i < 16; i++) {
		for (int k = 0; k < 8; k++) {
			next_byte = (next_byte << 1) | (*rnd_reg & 1);
		}

		random ^= next_byte;
		random *= 0x01000193;
	}

	srand(random);
}