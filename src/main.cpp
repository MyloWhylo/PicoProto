#include <stdio.h>
#include <stdlib.h>

#include "headers/Logger.hpp"
extern Logger myLogger;  // Allows for different debug levels

#include "Animations/BlinkAnim.hpp"
#include "Animations/GlitchAnim.hpp"
// #include "BlinkAnim.hpp"
// #include "GlitchAnim.hpp"
#include "Icons.hpp"
#include "hardware/gpio.h"
#include "hardware/regs/rosc.h"
#include "hardware/uart.h"
#include "headers/APDS9960.hpp"
#include "headers/BoopSensor.hpp"
#include "headers/CheekFinAnimator.hpp"
#include "headers/Emotion.hpp"
#include "headers/FanController.hpp"
#include "headers/HiFiProtogenPinout.hpp"
#include "headers/Max7219Driver.hpp"
#include "headers/SK6812.hpp"
#include "headers/TinyLED.hpp"
// #include "headers/Badger.hpp"
// #include "headers/PolledBoopCode.hpp"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "pico/time.h"

Max7219Driver myDriver(1);  // Initialize with brightness 1
FanController myFan(1.0);   // Initialize with fan on full
BoopSensor myBoopSensor;

CheekFinAnimator cheekAnim;
TinyLED statusLED(true);

// -------- Emotes for the face --------
Emotion Normal("Normal", eye, nose, maw);
Emotion Suprise("Suprised", Spooked, nose, maw, true, false);
Emotion VwV("VwV", vwv, nose, maw, false, false);

Emotion allEmotes[] = {Normal, Suprise, VwV};

Emotion *currentAnim = &Normal;
uint8_t emote = 0;

BlinkAnimation blink(&currentAnim);    // blinkAnim is located in BlinkAnim.hpp
GlitchAnimation glitch(&currentAnim);  // glitchAnim is located in GlitchAnim.hpp

void seed_random_from_rosc();
bool randomChangeToGlitch();

void normal_state();
// void handle_glitch();
// void handle_normal();
// void handle_blink();

#define BOOTDONE_FLAG 0xA5

typedef enum {
	INIT,
	NORMAL,
	VISOR_OFF,
	VISOR_WAIT
} LOOP_STATE;

int main() {
	bool boopExists = false;

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

#ifndef NDEBUG
	while (!tud_cdc_connected()) {  // Wait for USB to connect
		sleep_ms(100);
		statusLED.setColor(1.0, 1.0, 0.0);
		sleep_ms(100);
		statusLED.setColor(0.0, 0.0, 0.0);
	}
#endif

	// boopExists = myBoopSensor.begin();
	sleep_ms(1500);
	statusLED.setColor(0.0f, 0.25f, 0.0f);

	absolute_time_t nextRandomGlitch;
	bool randomGlitchScheduled = false;
	Emotion *prevEmote;

	bool runOnce = false;

	LOOP_STATE state = INIT;
	while (true) {
		switch (state) {
			case INIT:
				myFan.setSpeed(1.0f);
				cheekAnim.bootAnimation();

				currentAnim->drawAll();  // Draw all of animation to framebuffer
				myDriver.display();      // Display framebuffer on displays
				myDriver.turnOn();
				state = NORMAL;
				break;

			case NORMAL:
				cheekAnim.update();

				if (!glitch.isRunning()) {
					blink.update();
				}

				glitch.update();

				// if (boopExists) {
				// 	myBoopSensor.update();
				// 	if (!myBoopSensor.isVisorOn()) {
				// 		state = VISOR_OFF;
				// 		break;
				// 	}

				// 	float brightness = myBoopSensor.getBrightness();
				// 	if (brightness >= 100.0f)
				// 		myDriver.setBrightness(15);
				// 	else
				// 		myDriver.setBrightness(0);
				// }

				if (currentAnim == &Normal) {
					// if (myBoopSensor.isBooped()) {
					// 	if (blink.isScheduled()) blink.stopAnimation();
					// 	currentAnim = &VwV;
					// 	currentAnim->drawAll();
					// 	myDriver.display();
					// } else {
						if (!randomGlitchScheduled) {
							uint32_t nextGlitch = (rand() & 0x2FFF) + 300000;
							glitch.scheduleAnimation(nextGlitch);  // Start glitch animation
							myLogger.logDebug("setting next random glitch for %u ms in the future\n", nextGlitch);
							randomGlitchScheduled = true;
						}
					// }
				} else if (currentAnim == &VwV) {
					if (!myBoopSensor.isBooped()) {
						currentAnim = &Normal;
						currentAnim->drawAll();
						myDriver.display();
					}
				}

				if (currentAnim->canBlink && !blink.isScheduled()) {
					int nextBlink = (rand() & 0x0FFF) + 2000;  // This is an awful random number generator. But, who cares.
					myLogger.logDebug("setting next blink for %d ms in the future\n", nextBlink);
					blink.scheduleAnimation(nextBlink);  // Start blink animation
				}

				// if (currentAnim->canGlitch && !glitch.isScheduled()) {
				// 	int nextGlitch = (rand() & 0x03FF) + 1000;  // This is an awful random number generator. But, who cares.
				// 	myLogger.logDebug("setting next glitch for %d ms in the future\n", nextGlitch);
				// 	glitch.scheduleAnimation(nextGlitch);  // Start glitch animation
				// }

				if (glitch.isRunning()) {
					if (runOnce == false) {
						prevEmote = currentAnim;
						currentAnim = &Suprise;

						currentAnim->drawEyes();
						myDriver.display();

						cheekAnim.setRGB(255, 0, 0);
						cheekAnim.setCycleTime(1.0f);
						cheekAnim.setDirection(true);

						runOnce = true;
					}
				} else {
					if (runOnce) {
						currentAnim = prevEmote;
						currentAnim->drawAll();
						myDriver.display();

						cheekAnim.setRGB(GOLD_COLOR_R, GOLD_COLOR_G, GOLD_COLOR_B);
						cheekAnim.setCycleTime(ANIMATION_TIME);
						cheekAnim.setDirection(false);
						runOnce = false;
						randomGlitchScheduled = false;
					}
				}
				break;

			case VISOR_OFF:
				myFan.setSpeed(0);
				myDriver.turnOff();
				cheekAnim.clear();
				state = VISOR_WAIT;
				break;

			case VISOR_WAIT:
				myBoopSensor.update();
				if (myBoopSensor.isVisorOn()) state = INIT;
				break;

			default:
				state = INIT;
				break;
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
