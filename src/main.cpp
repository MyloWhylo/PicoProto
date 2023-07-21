#include <stdio.h>
#include <stdlib.h>

#include "headers/Logger.hpp"
extern Logger myLogger;  // Allows for different debug levels

#include "BlinkAnim.hpp"
#include "GlitchAnim.hpp"
#include "Icons.hpp"
#include "hardware/gpio.h"
#include "hardware/regs/rosc.h"
#include "hardware/uart.h"
#include "headers/CheekFinAnimator.hpp"
#include "headers/Emotion.hpp"
#include "headers/FanController.hpp"
#include "headers/HiFiProtogenPinout.hpp"
#include "headers/Max7219Driver.hpp"
#include "headers/SK6812.hpp"
#include "headers/TinyLED.hpp"
// #include "headers/Badger.hpp"
// #include "headers/PolledBoopCode.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"

Max7219Driver myDriver(1);  // Initialize with brightness 1
FanController myFan(1);     // Initialize with fan on full

CheekFinAnimator cheekAnim;
TinyLED statusLED(true);

// -------- Emotes for the face --------
Emotion Normal("Normal", eye, nose, maw);
Emotion Suprise("Suprised", Spooked, nose, maw, true, false);
Emotion VwV("VwV", vwv, nose, maw, false, false);

Emotion allEmotes[] = {Normal, Suprise, VwV};

Emotion *currentAnim = &Normal;
uint8_t emote = 0;

Animation blinkAnimation(blinkAnim, &currentAnim);    // blinkAnim is located in BlinkAnim.hpp
Animation glitchAnimation(glitchAnim, &currentAnim);  // glitchAnim is located in glitchAnim.hpp

void seed_random_from_rosc();
bool randomChangeToGlitch();

// void core1Task();

#define BOOTDONE_FLAG 0xA5

int main() {
	statusLED.setColor(1.0, 1.0, 0.0);
	seed_random_from_rosc();  // Seed random for eye blinks
	stdio_init_all();         // Initialize STDIO

	if (myDriver.initializeDisplays()) {  // Clear whatever might be on the face matrices
		myLogger.log("Displays successfully initiailized!\n");
		myLogger.log("Actual SPI baudrate set: %u\n", spi_get_baudrate(FACE_SPI_INST));
	} else {
		myLogger.log("Displays failed to initialize!");
		while (true) {
			statusLED.setColor(1.0f, 0.0f, 0.0f);
			sleep_ms(500);
			statusLED.setColor(0.0f, 0.0f, 0.0f);
			sleep_ms(500);
		}
	}

	// while (!tud_cdc_connected()) {  // Wait for USB to connect
	// 	sleep_ms(100);
	// 	statusLED.setColor(1.0, 1.0, 0.0);
	// 	sleep_ms(100);
	// 	statusLED.setColor(0.0, 0.0, 0.0);
	// }

	sleep_ms(1500);
	cheekAnim.bootAnimation();
	// multicore_launch_core1(core1Task);

	// uint thing = multicore_fifo_pop_blocking();
	// if (thing != BOOTDONE_FLAG) {
	// 	myLogger.log("bootanimation failed!");
	// 	while (true) {
	// 		statusLED.setColor(1.0f, 0.0f, 0.0f);
	// 		sleep_ms(250);
	// 		statusLED.setColor(0.0f, 0.0f, 0.0f);
	// 		sleep_ms(250);
	// 	}
	// }

	statusLED.setColor(0.0f, 0.25f, 0.0f);

	currentAnim->drawAll();  // Draw all of animation to framebuffer
	myDriver.display();      // Display framebuffer on displays

	absolute_time_t nextRandomGlitch;
	bool randomGlitchScheduled = false;
	while (true) {
		cheekAnim.update();
		if (currentAnim == &Normal) {
			if (!randomGlitchScheduled) {
				uint32_t nextGlitch = (rand() & 0x2FFF) + 60000;
				nextRandomGlitch = make_timeout_time_ms(nextGlitch);
				myLogger.logDebug("setting next random glitch for %u ms in the future\n", nextGlitch);
				randomGlitchScheduled = true;
			} else {
				if (absolute_time_diff_us(nextRandomGlitch, get_absolute_time()) >= 0) {
					while (randomChangeToGlitch()) {
						tight_loop_contents;
					}
					sleep_ms(2000);
					randomGlitchScheduled = false;
				}
			}
		}

		if (currentAnim->canBlink && !blinkAnimation.isScheduled()) {
			int nextBlink = (rand() & 0x0FFF) + 2000;  // This is an awful random number generator. But, who cares.
			myLogger.logDebug("setting next blink for %d ms in the future\n", nextBlink);
			blinkAnimation.scheduleAnimation(nextBlink);  // Start blink animation
		}

		if (currentAnim->canGlitch && !glitchAnimation.isScheduled()) {
			int nextGlitch = (rand() & 0x03FF) + 1000;  // This is an awful random number generator. But, who cares.
			myLogger.logDebug("setting next glitch for %d ms in the future\n", nextGlitch);
			glitchAnimation.scheduleAnimation(nextGlitch);  // Start glitch animation
		}
	}
}

// void core1Task() {
// 	cheekAnim.bootAnimation();
// 	multicore_fifo_push_blocking(BOOTDONE_FLAG);
// 	while (1)
// 		tight_loop_contents();
// }

// -------- Copied from the pi pico forum, seeds random from rosc --------
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

bool randomChangeToGlitch() {
	static bool glitchRan = false;
	static Emotion *prevEmote;

	if (glitchRan) {
		if (!glitchAnimation.isScheduled()) {
			currentAnim = prevEmote;
			currentAnim->drawAll();
			myDriver.display();
			cheekAnim.setRGB(GOLD_COLOR_R, GOLD_COLOR_G, GOLD_COLOR_B);
			glitchRan = false;

			return false;
		}
	} else {
		prevEmote = currentAnim;
		currentAnim = &Suprise;

		currentAnim->drawAll();
		myDriver.display();

		cheekAnim.setRGB(255, 0, 0);

		glitchAnimation.scheduleAnimation(1000);
		glitchRan = true;
	}

	return true;
}