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

inline void CheekFinAnimator::setRightCheekColor(uint led, bool respectFront, uint32_t hex) {
	this->ourString.setPixel(led, hex);
}

inline void CheekFinAnimator::setLeftCheekColor(uint led, bool respectFront, uint32_t hex) {
	if (respectFront) {
		if (led == 0)
			this->ourString.setPixel(NUM_CHEEK_LEDS - LEDS_PER_PANEL, hex);
		else
			this->ourString.setPixel(NUM_CHEEK_LEDS - led, hex);
	} else {
		this->ourString.setPixel(led + LEDS_PER_PANEL, hex);
	}
}

inline void CheekFinAnimator::setRightCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
	this->ourString.setPixel(led, r, g, b);
}

inline void CheekFinAnimator::setLeftCheekColor(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
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

inline float CheekFinAnimator::animationFunction(float step, uint led) {
	float cosVal = cosf((M_PI * (step + led)) / (8.0f / 3.0f)) / 2.0f;
	return cosVal + 0.5f;
}

void CheekFinAnimator::setRGB(uint8_t r, uint8_t g, uint8_t b) {
	this->colorR = r;
	this->colorG = g;
	this->colorB = b;
}

void CheekFinAnimator::update() {
	absolute_time_t currentTime = get_absolute_time();
	float currentUS = to_us_since_boot(get_absolute_time()) / 1e6;

	float currentStep = (fmodf(currentUS, this->animationCycleTime) * (ANIMATION_STEP_MAX / this->animationCycleTime));

	if (this->direction == false) {
		currentStep = ANIMATION_STEP_MAX - currentStep;
	} 

	for (uint ii = 0; ii < LEDS_PER_PANEL; ii++) {
		float thisStep = this->animationFunction(currentStep, ii);
		uint8_t goldR = this->colorR * thisStep;
		uint8_t goldG = this->colorG * thisStep;
		uint8_t goldB = this->colorB * thisStep;
		this->setSameOnBothCheeks(ii, true, goldR, goldG, goldB);
	}

	ourString.display();
}


void CheekFinAnimator::setCycleTime(float cycleTime) {
	this->animationCycleTime = cycleTime;

}
void CheekFinAnimator::setDirection(bool clockwise){
	this->direction = clockwise;
}

void CheekFinAnimator::clear() {
	this->ourString.clear();
	this->ourString.display();
}