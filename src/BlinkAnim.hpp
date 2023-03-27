#include "headers/Animation.hpp"
#include "Icons.hpp"
#include "headers/Max7219Driver.hpp"
#include "headers/Emotion.hpp"

#define BLINK_CYCLE_TIME -50000
#define BLINK_STEP_INDEX 0

int64_t blinkAnim(alarm_id_t id, void* user_data) {
	Animation* anim = (Animation*) user_data;
   Emotion* emote = *(anim->emote);
   Max7219Driver* driver = emote->driver;
   
	uint8_t blinkStep = anim->getTimer(BLINK_STEP_INDEX);

   if (blinkStep == 0) anim->setRunning(true);

	if (blinkStep < 8) {
		emote->drawEyes();

		for (int ii = 0; ii < blinkStep + 1; ii++) {
			driver->setSegment(7 - ii, 0, 0);
			driver->setSegment(7 - ii, 1, 0);
			driver->setSegment(7 - ii, 12, 0);
			driver->setSegment(7 - ii, 13, 0);
		}
	} else if (blinkStep == 8) {

	} else if (blinkStep > 8) {
		emote->drawEyes();

		for (int ii = 7; ii > (blinkStep - 9); ii--) {
			driver->setSegment(ii, 0, 0);
			driver->setSegment(ii, 1, 0);
			driver->setSegment(ii, 12, 0);
			driver->setSegment(ii, 13, 0);
		}
	}

	driver->display();

	if (blinkStep != 17) {
		anim->incTimer(BLINK_STEP_INDEX);
		return BLINK_CYCLE_TIME;
	} else {
      anim->setRunning(false);
		anim->clearTimer(BLINK_STEP_INDEX);
		return 0;
	}
}