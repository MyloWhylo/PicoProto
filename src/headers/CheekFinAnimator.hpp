#ifndef __CHEEK_ANIMATOR_H
#define __CHEEK_ANIMATOR_H

#include <stdint.h>

#include "HiFiProtogenPinout.hpp"
#include "Logger.hpp"
#include "SK6812.hpp"
#include "math.h"
#include "pico/time.h"
#include "stdint.h"

#define LEDS_PER_PANEL 16
#define NUM_CHEEK_PANELS 2
#define NUM_CHEEK_LEDS LEDS_PER_PANEL* NUM_CHEEK_PANELS
#define MIDDLE_PIXEL LEDS_PER_PANEL / 2
#define ANIMATION_TIME 3.0f
#define ANIMATION_STEP_MAX 16.0f

#define CHEEK_UPDATE_INTERVAL 1000000/120

// #define GOLD_COLOR 0xD4AF37
// #define GOLD_COLOR_R 0xD4
// #define GOLD_COLOR_G 0xAF
// #define GOLD_COLOR_B 0x37

#define GOLD_COLOR 0xFFD242
#define GOLD_COLOR_R 0xFF  // 255
#define GOLD_COLOR_G 0xD2  // 210
#define GOLD_COLOR_B 0x42  // 66

bool updateLEDs(repeating_timer_t* rt);

class CheekFinAnimator {
  private:
	PixelString ourString = PixelString(NUM_CHEEK_LEDS, CHEEK_RING_PIN, true);
	repeating_timer_t thisTimer;
	alarm_pool_t* thisPool;

	absolute_time_t nextUpdateTime = nil_time;

	uint8_t colorR = GOLD_COLOR_R;
	uint8_t colorG = GOLD_COLOR_G;
	uint8_t colorB = GOLD_COLOR_B;

	float animationCycleTime = ANIMATION_TIME;
	bool direction = false;

	inline void setSameOnBothCheeks(int led, bool respectFront, uint32_t hex);
	inline void setRightCheekColor(uint led, bool respectFront, uint32_t hex);
	inline void setLeftCheekColor(uint led, bool respectFront, uint32_t hex);

	inline void setSameOnBothCheeks(int led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);
	inline void setRightCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);
	inline void setLeftCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b);

	inline float animationFunction(float step, uint led);

  public:
	CheekFinAnimator();
	~CheekFinAnimator();
	void update();

	void clear();

	void setRGB(uint8_t r, uint8_t g, uint8_t b);
	void setCycleTime(float cycleTime);
	void setDirection(bool clockwise);

	void bootAnimation();
};

bool updateLEDs(repeating_timer_t* rt);

#endif