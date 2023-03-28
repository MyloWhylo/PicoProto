#include <stdio.h>
#include <stdlib.h>

#include "BlinkAnim.hpp"
#include "GlitchAnim.hpp"
#include "Icons.hpp"
#include "hardware/regs/rosc.h"
#include "hardware/sync.h"
#include "headers/Emotion.hpp"
#include "headers/FanController.hpp"
#include "headers/Logger.hpp"
#include "headers/Max7219Driver.hpp"
#include "headers/PolledBoopCode.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"

extern Logger myLogger;     // Allows for different debug levels
Max7219Driver myDriver(1);  // Initialize with brightness 1
FanController myFan(16, 0.5);

// -------- Emotes for the face --------
Emotion Normal("Normal", eye, nose, maw);
Emotion Suprise("Suprised", Spooked, nose, maw);
Emotion VwV("VwV", vwv, nose, maw);

Emotion *currentAnim = &Normal;
uint8_t emote = 0;

Animation blinkAnimation(blinkAnim, &currentAnim);    // blinkAnim is located in BlinkAnim.hpp
Animation glitchAnimation(glitchAnim, &currentAnim);  // glitchAnim is located in glitchAnim.hpp

// -------- BoopCode thing? --------
PolledBoopCode myBooper;

void seed_random_from_rosc();
bool emoteChange(repeating_timer_t *rt);

bool glitchy = false;

int main() {
	seed_random_from_rosc();  // Seed random for eye blinks
	stdio_init_all();         // Initialize STDIO

	// while (!tud_cdc_connected()) {  // Wait for USB to connect
	// 	sleep_ms(100);
	// }

	if (myDriver.initializeDisplays()) {
		myLogger.log("Displays successfully initiailized!\n");
	} else {
		myLogger.log("Displays failed to initialize!");
	}

	myLogger.log("Actual SPI baudrate set: %u\n", spi_get_baudrate(spi0));

	currentAnim->drawAll();  // Draw all of animation to framebuffer
	myDriver.display();      // Display framebuffer on displays
	myBooper.start();        // Start BoopCode protocol

	repeating_timer_t changeTimer;                                 // Timer for emote cycle demo
	add_repeating_timer_ms(-5000, emoteChange, 0, &changeTimer);  // Schedule emote cycle

	while (true) {
		if (!blinkAnimation.isScheduled()) {
			int nextBlink = (rand() & 0x0FFF) + 2000;  // This is an awful random number generator. But, who cares.
			myLogger.logDebug("setting next blink for %d ms in the future\n", nextBlink);
			blinkAnimation.scheduleAnimation(nextBlink);  // Start blink animation
		}

		if (glitchy && !glitchAnimation.isScheduled()) {
			int nextGlitch = (rand() & 0x03FF) + 1000;  // This is an awful random number generator. But, who cares.
			myLogger.logDebug("setting next glitch for %d ms in the future\n", nextGlitch);
			glitchAnimation.scheduleAnimation(nextGlitch);  // Start glitch animation
		}
	}
	myBooper.stop();  // Stop BoopCode Protocol
}

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

/* \brief Handles the emote cycling demo.
 * \param rt Repeating timer instance, passed automatically
 * \return Whether or not to re-run the function
 */
bool emoteChange(repeating_timer_t *rt) {
	switch (emote) {
		case 0:
			currentAnim = &Normal;
			emote++;
			myFan.stopFan();
			break;

		case 1:
			currentAnim = &Suprise;
			emote++;
			myFan.startFan();
			break;

		case 2:
			currentAnim = &VwV;
			emote++;
			break;

		case 3:
			glitchy = !glitchy;
			emote = 0;
			break;

		default:
			emote = 0;
			break;
	}

	bool drawEyes = true;
	bool drawMaw = true;
	bool drawNose = true;

	if (blinkAnimation.isRunning()) {
		drawEyes = false;
	}

	if (glitchAnimation.isRunning()) {
		drawMaw = false;
	}

	if (drawEyes) currentAnim->drawEyes();
	if (drawMaw) currentAnim->drawMaw();
	if (drawNose) currentAnim->drawNose();

	myDriver.display();

	return true;
}
