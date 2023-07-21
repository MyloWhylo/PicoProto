#include "../headers/CheekFinAnimator.hpp"
extern Logger myLogger;

CheekFinAnimator::CheekFinAnimator() {
	myLogger.logDebug("\nInitializing Cheek Fin Animator\n");
	this->ourString.init();
	this->ourString.setBrightness(256);
	this->ourString.clear();
	this->ourString.display();
}

CheekFinAnimator::~CheekFinAnimator() {
}

inline void CheekFinAnimator::setSameOnBothCheeks(int led, bool respectFront, uint32_t hex) {
	this->setLeftCheekColor(led, respectFront, hex);
	this->setRightCheekColor(led, respectFront, hex);
}

inline void CheekFinAnimator::setSameOnBothCheeks(int led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
	this->setLeftCheekColor(led, respectFront, r, g, b);
	this->setRightCheekColor(led, respectFront, r, g, b);
}

void CheekFinAnimator::setRightCheekColor(uint led, bool respectFront, uint32_t hex) {
	this->ourString.setPixel(led, hex);
}

void CheekFinAnimator::setLeftCheekColor(uint led, bool respectFront, uint32_t hex) {
	if (respectFront) {
		if (led == 0)
			this->ourString.setPixel(NUM_CHEEK_LEDS - LEDS_PER_PANEL, hex);
		else
			this->ourString.setPixel(NUM_CHEEK_LEDS - led, hex);
	} else {
		this->ourString.setPixel(led + LEDS_PER_PANEL, hex);
	}
}

void CheekFinAnimator::setRightCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
	this->ourString.setPixel(led, r, g, b);
}

void CheekFinAnimator::setLeftCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
	if (respectFront) {
		if (led == 0)
			this->ourString.setPixel(NUM_CHEEK_LEDS - LEDS_PER_PANEL, r, g, b);
		else
			this->ourString.setPixel(NUM_CHEEK_LEDS - led, r, g, b);
	} else {
		this->ourString.setPixel(led + LEDS_PER_PANEL, r, g, b);
	}
}

void CheekFinAnimator::bootAnimation() {
	for (int activePixel = 0; activePixel < LEDS_PER_PANEL; activePixel++) {
		for (float step = 0.0f; step < 1.0f; step += 0.05f) {
			ourString.clear();

			for (int ii = 0; ii < activePixel; ii++) {
				this->setSameOnBothCheeks(ii, true, GOLD_COLOR);
			}

			uint8_t goldR = GOLD_COLOR_R * step;
			uint8_t goldG = GOLD_COLOR_G * step;
			uint8_t goldB = GOLD_COLOR_B * step;

			this->setSameOnBothCheeks(activePixel, true, goldR, goldG, goldB);

			ourString.display();
			sleep_ms(1);
		}
	}

	for (int activePixel = 0; activePixel < LEDS_PER_PANEL; activePixel++) {
		for (float step = 1.0f; step >= 0.0f; step -= 0.05f) {
			ourString.clear();

			uint8_t goldR = GOLD_COLOR_R * step;
			uint8_t goldG = GOLD_COLOR_G * step;
			uint8_t goldB = GOLD_COLOR_B * step;

			this->setSameOnBothCheeks(activePixel, true, goldR, goldG, goldB);

			for (int ii = activePixel + 1; ii < LEDS_PER_PANEL; ii++) {
				this->setSameOnBothCheeks(ii, true, GOLD_COLOR);
			}

			ourString.display();
			sleep_ms(1);
		}
	}

	// myLogger.logDebug("Cheek anim running on core %d\n", get_core_num());
	// myLogger.logTrace("Adding Repeating Timer\n");
	// this->thisPool = alarm_pool_create_with_unused_hardware_alarm(1);
	// alarm_pool_add_repeating_timer_ms(this->thisPool, -5, updateLEDs, this, &(this->thisTimer));  // Schedule emote cycle

	// sleep_ms(250);
	// for (int ii = 0; ii < NUM_CHEEK_LEDS; ii++) {
	// 	ourString.setPixelRaw(ii, 0xFFFFFFFF);
	// }
	// ourString.display();
}

inline float CheekFinAnimator::animationFunction(int64_t step, uint led) {
	float xVal = (step / (10e6 / ANIMATION_STEP_MAX)) + led;
	float cosVal = cosf((M_PI * xVal) / 2.666666666666666) / 2.0f;

	return cosVal + 0.5f;
}

void CheekFinAnimator::setRGB(uint8_t r, uint8_t g, uint8_t b) {
	this->colorR = r;
	this->colorG = g;
	this->colorB = b;
}

void CheekFinAnimator::update() {
	// static float step = 15.0f;
	// static bool printText = true;
	// static absolute_time_t prevTime = nil_time;

	// absolute_time_t currentTime = get_absolute_time();
	int64_t currentUS = to_us_since_boot(get_absolute_time()) % ((int64_t)(ANIMATION_TIME * 1e6));

	// if (printText) {
	// 	myLogger.logDebug("Cheek anim running on core %d\n", get_core_num());
	// 	printText = false;
	// }

	for (int ii = 0; ii < LEDS_PER_PANEL; ii++) {
		float thisStep = this->animationFunction(currentUS, ii);
		uint8_t goldR = this->colorR * thisStep;
		uint8_t goldG = this->colorG * thisStep;
		uint8_t goldB = this->colorB * thisStep;
		this->setSameOnBothCheeks(ii, true, goldR, goldG, goldB);
	}

	// step -= 0.01f;

	// if (step < 0.0f) {
	// 	step = 15.0f;
	// }

	ourString.display();
}

bool updateLEDs(repeating_timer_t* rt) {
	CheekFinAnimator* ourAnimator = (CheekFinAnimator*)rt->user_data;
	ourAnimator->update();
	return true;
}