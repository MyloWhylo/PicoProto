#include "../headers/CheekFinAnimator.hpp"
extern Logger myLogger;

CheekFinAnimator::CheekFinAnimator() {
	myLogger.logDebug("\nInitializing Cheek Fin Animator\n");
	this->ourString.init();
	this->ourString.clear();
	this->ourString.display();
}

CheekFinAnimator::~CheekFinAnimator() {
}

inline void CheekFinAnimator::setSameOnBothCheeks(uint led, bool respectFront, uint32_t hex) {
	this->setLeftCheekColor(led, respectFront, hex);
	this->setRightCheekColor(led, respectFront, hex);
}

inline void CheekFinAnimator::setSameOnBothCheeks(uint led, bool respectFront, uint8_t r, uint8_t g, uint8_t b) {
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
		for (float step = 0.0f; step < 1.0f; step += 0.01f) {
			ourString.clear();

			for (int ii = 0; ii < activePixel; ii++) {
				this->setSameOnBothCheeks(ii, true, GOLD_COLOR);
			}

			uint8_t goldR = GOLD_COLOR_R * step;
			uint8_t goldG = GOLD_COLOR_G * step;
			uint8_t goldB = GOLD_COLOR_B * step;

			this->setSameOnBothCheeks(activePixel, true,  goldR, goldG, goldB);

			ourString.display();
			sleep_ms(1);
		}
	}

	for (int activePixel = 0; activePixel < LEDS_PER_PANEL; activePixel++) {
		for (float step = 1.0f; step >= 0.0f; step -= 0.01f) {
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

	myLogger.logTrace("Adding Repeating Timer\n");
	add_repeating_timer_ms(-100, updateLEDs, this, &(this->thisTimer));  // Schedule emote cycle

	// sleep_ms(250);
	// for (int ii = 0; ii < NUM_CHEEK_LEDS; ii++) {
	// 	ourString.setPixelRaw(ii, 0xFFFFFFFF);
	// }
	// ourString.display();
}

void CheekFinAnimator::update() {
	uint8_t setColor;
	uint8_t clearColor;

	this->ourString.clear();

	this->setSameOnBothCheeks(this->currentLED, false, GOLD_COLOR);

	if (this->currentLED < 15)
		this->currentLED++;
	else
		this->currentLED = 0;

	ourString.display();
}

bool updateLEDs(repeating_timer_t* rt) {
	CheekFinAnimator* ourAnimator = (CheekFinAnimator*)rt->user_data;
	ourAnimator->update();
	return true;
}