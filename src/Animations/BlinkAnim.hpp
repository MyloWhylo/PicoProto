#include "../Icons.hpp"
#include "../headers/Animation.hpp"
#include "../headers/Logger.hpp"
#include "../headers/Max7219Driver.hpp"
#include "pico/time.h"
#include "stdlib.h"

#define BLINK_CYCLE_TIME 50000
#define BLINK_STEP_INDEX 0

class BlinkAnimation : public Animation {
  protected:
	void runAnimUpdate();

  public:
	using Animation::Animation;
};

// This function drives the blink animation. It gets called repeatedly until it's finished.
// int64_t blinkAnim(alarm_id_t id, void* user_data) {
// 	// ---- Break out all of the data needed to update the blink ----
// 	Animation* anim = (Animation*)user_data;
// 	Emotion* emote = *(anim->emote);
// 	Max7219Driver* driver = emote->driver;

// 	uint8_t blinkStep = anim->getTimer(BLINK_STEP_INDEX);  // What step are we on?

// 	if (blinkStep == 0) anim->setRunning(true);  // If this is the first step, set that we're running

// 	if (blinkStep < 8) {   // Draw the blink going downward
// 		emote->drawEyes();  // Drawing the eyes here allows us to switch animations while blinking

// 		for (int ii = 0; ii < blinkStep + 1; ii++) {  // Blank the rows in the framebuffer
// 			driver->setSegment(7 - ii, 0, 0);
// 			driver->setSegment(7 - ii, 1, 0);
// 			driver->setSegment(7 - ii, 12, 0);
// 			driver->setSegment(7 - ii, 13, 0);
// 		}
// 	} else if (blinkStep == 8) {  // Wait a cycle
// 	} else if (blinkStep > 8) {   // Draw the blink going upward
// 		emote->drawEyes();         // Drawing the eyes here allows us to switch animations while blinking

// 		for (int ii = 7; ii > (blinkStep - 9); ii--) {  // Blank the rows in the framebuffer
// 			driver->setSegment(ii, 0, 0);
// 			driver->setSegment(ii, 1, 0);
// 			driver->setSegment(ii, 12, 0);
// 			driver->setSegment(ii, 13, 0);
// 		}
// 	}

// 	driver->display();  // Display the updated eyes on the displays

// 	if (blinkStep < 17) {  // If the animation isn't done yet, increment timer and reschedule
// 		anim->incTimer(BLINK_STEP_INDEX);
// 		return BLINK_CYCLE_TIME;
// 	} else {  // Otherwise, set that we're stopped, clear the timer, and don't reschedule
// 		anim->setRunning(false);
// 		anim->clearTimer(BLINK_STEP_INDEX);
// 		return 0;
// 	}
// }