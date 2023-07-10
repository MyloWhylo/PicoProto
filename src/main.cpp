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

Max7219Driver myDriver(15);  // Initialize with brightness 1
// FanController myFan(FAN_PWM, 50);

CheekFinAnimator cheekAnim;
TinyLED statusLED(true);

// -------- Emotes for the face --------
Emotion Normal("Normal", eye, nose, maw);
Emotion Suprise("Suprised", Spooked, nose, maw);
Emotion VwV("VwV", vwv, nose, maw);

Emotion allEmotes[] = {Normal, Suprise, VwV};

Emotion *currentAnim = &Normal;
uint8_t emote = 0;

Animation blinkAnimation(blinkAnim, &currentAnim);    // blinkAnim is located in BlinkAnim.hpp
Animation glitchAnimation(glitchAnim, &currentAnim);  // glitchAnim is located in glitchAnim.hpp

// -------- BoopCode thing? --------
// PolledBoopCode myBooper;

void seed_random_from_rosc();
void emoteChange();
bool sendMsg(repeating_timer_t *rt);

bool glitchy = false;
volatile bool linkEstablished = false;

int main() {
	seed_random_from_rosc();  // Seed random for eye blinks
	stdio_init_all();         // Initialize STDIO

	// while (!tud_cdc_connected()) {  // Wait for USB to connect
	// 	sleep_ms(100);
	// }
	sleep_ms(1000);
	statusLED.setColor(1.0, 1.0, 0.0);
	cheekAnim.bootAnimation();

	if (myDriver.initializeDisplays()) {
		myLogger.log("Displays successfully initiailized!\n");
	} else {
		myLogger.log("Displays failed to initialize!");
		while (true) {
			statusLED.setColor(1.0f, 0.0f, 0.0f);
			sleep_ms(500);
			statusLED.setColor(0.0f, 0.0f, 0.0f);
			sleep_ms(500);
		}
	}

	statusLED.setColor(0.0f, 0.25f, 0.0f);

	myLogger.log("Actual SPI baudrate set: %u\n", spi_get_baudrate(spi0));

	currentAnim->drawAll();  // Draw all of animation to framebuffer
	myDriver.display();      // Display framebuffer on displays

	// repeating_timer_t linkStart;                           // Timer for emote cycle demo
	// add_repeating_timer_ms(-500, sendMsg, 0, &linkStart);  // Schedule emote cycle

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
// void emoteChange() {
// 	switch (emote) {
// 		case 0:
// 			currentAnim = &Normal;
// 			break;

// 		case 1:
// 			currentAnim = &Suprise;
// 			break;

// 		case 2:
// 			currentAnim = &VwV;
// 			break;

// 		default:
// 			currentAnim = &Normal;
// 			emote = 0;
// 			break;
// 	}

// 	bool drawEyes = true;
// 	bool drawMaw = true;
// 	bool drawNose = true;

// 	if (blinkAnimation.isRunning()) {
// 		drawEyes = false;
// 	}

// 	if (glitchAnimation.isRunning()) {
// 		drawMaw = false;
// 	}

// 	if (drawEyes) currentAnim->drawEyes();
// 	if (drawMaw) currentAnim->drawMaw();
// 	if (drawNose) currentAnim->drawNose();

// 	myDriver.display();
// }

// bool sendMsg(repeating_timer_t *rt) {
// 	if (linkEstablished) return false;
// 	myLogger.logDebug("Sending character\n");
// 	uart_putc_raw(uart0, 'B');
// 	return true;
// }