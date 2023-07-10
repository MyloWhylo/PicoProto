#ifndef __CHEEK_ANIMATOR_H
#define __CHEEK_ANIMATOR_H

#include <stdint.h>

#include "Logger.hpp"
#include "SK6812.hpp"
#include "pico/time.h"
#include "HiFiProtogenPinout.hpp"

#define LEDS_PER_PANEL 16
#define NUM_CHEEK_PANELS 2
#define NUM_CHEEK_LEDS LEDS_PER_PANEL* NUM_CHEEK_PANELS
#define MIDDLE_PIXEL LEDS_PER_PANEL / 2
#define ANIMATION_TIME 1000.0f

#define GOLD_COLOR 0xD4AF37
#define GOLD_COLOR_R 0xD4
#define GOLD_COLOR_G 0xAF
#define GOLD_COLOR_B 0x37

// #define GOLD_COLOR 0xFFD75F
// #define GOLD_COLOR_R 0xFF
// #define GOLD_COLOR_G 0xD7
// #define GOLD_COLOR_B 0x5F

bool updateLEDs(repeating_timer_t* rt);

class CheekFinAnimator {
  private:
	PixelString ourString = PixelString(NUM_CHEEK_LEDS, CHEEK_RING_PIN, true);
	uint currentLED = 0;
	uint currentStep = 0;
	bool decrement = false;
	repeating_timer_t thisTimer;

	inline void setSameOnBothCheeks(uint led, bool respectFront, uint32_t hex);
	void setRightCheekColor(uint led, bool respectFront, uint32_t hex);
	void setLeftCheekColor(uint led, bool respectFront, uint32_t hex);

	inline void setSameOnBothCheeks(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);
	void setRightCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);
	void setLeftCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);


  public:
	CheekFinAnimator();
	~CheekFinAnimator();
	void update();

	void bootAnimation();
};

bool updateLEDs(repeating_timer_t* rt);

#endif