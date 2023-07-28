#include "../Icons.hpp"
#include "../headers/Animation.hpp"
#include "../headers/Logger.hpp"
#include "../headers/Max7219Driver.hpp"
#include "pico/time.h"
#include "stdlib.h"

#define GLITCH_BASE_TIME 500000
#define GLITCH_STEP_INDEX 0
#define GLITCH_START_INDEX 1

class GlitchAnimation : public Animation {
  protected:
	void runAnimUpdate();

  public:
	using Animation::Animation;
};

// // This function drives the glitch animation. It gets called repeatedly until it's finished.
// int64_t glitchAnim(alarm_id_t id, void* user_data) {
// 	// ---- Break out all of the data needed to update the glitch ----
// 	Animation* anim = (Animation*)user_data;
// 	Emotion* emote = *(anim->emote);
// 	Max7219Driver* driver = emote->driver;

//    static bool startWithOne = false;

// 	uint8_t glitchStep = anim->getTimer(GLITCH_STEP_INDEX);  // What step are we on?

// 	switch (glitchStep) {
// 		case 0:
// 			anim->setRunning(true);
// 			startWithOne = rand() % 2;

// 			if (startWithOne) {
// 				driver->displayBitmap(Glitch1, 4, 2, true, true);
// 				driver->displayBitmap(Glitch1, 4, 8, false, true);
// 			} else {
// 				driver->displayBitmap(Glitch2, 4, 2, true, true);
// 				driver->displayBitmap(Glitch2, 4, 8, false, true);
// 			}

// 			break;

// 		case 1:
// 			if (startWithOne) {
// 				driver->displayBitmap(Glitch2, 4, 2, true, true);
// 				driver->displayBitmap(Glitch2, 4, 8, false, true);
// 			} else {
// 				driver->displayBitmap(Glitch1, 4, 2, true, true);
// 				driver->displayBitmap(Glitch1, 4, 8, false, true);
// 			}

// 			break;

// 		default:
// 			emote->drawMaw();
// 			break;
// 	}

// 	driver->display();  // Display the updated maw on the displays

// 	if (glitchStep < 2) {  // If the animation isn't done yet, increment timer and reschedule
// 		anim->incTimer(GLITCH_STEP_INDEX);
// 		int32_t nextCycleTime = ((rand() & 0xFFFF) * -4) + GLITCH_BASE_TIME;

// 		if (nextCycleTime < 0.0f) nextCycleTime = -nextCycleTime;

// 		myLogger.logTrace("Next time: %u\n", -nextCycleTime);
// 		return nextCycleTime;
// 	} else {  // Otherwise, set that we're stopped, clear the timer, and don't reschedule
// 		anim->setRunning(false);
// 		anim->clearTimer(GLITCH_STEP_INDEX);
// 		return 0;
// 	}
// }