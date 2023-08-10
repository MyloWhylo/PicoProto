#include "GlitchAnim.hpp"

extern Logger myLogger;
extern Max7219Driver myDriver;

void GlitchAnimation::runAnimUpdate() {
   static absolute_time_t nextStepTime = nil_time;
   static int_fast8_t glitchStep = 0;
	static bool startWithOne = false;

   if (absolute_time_diff_us(nextStepTime, get_absolute_time()) < 0) return;

	switch (glitchStep) {
		case 1:
			this->animRunning = true;
			startWithOne = rand() % 2;

			if (startWithOne) {
				myDriver.displayBitmap(Glitch1, 4, 2, true, true);
				myDriver.displayBitmap(Glitch1, 4, 8, false, true);
			} else {
				myDriver.displayBitmap(Glitch2, 4, 2, true, true);
				myDriver.displayBitmap(Glitch2, 4, 8, false, true);
			}

			break;

		case 2:
			if (startWithOne) {
				myDriver.displayBitmap(Glitch2, 4, 2, true, true);
				myDriver.displayBitmap(Glitch2, 4, 8, false, true);
			} else {
				myDriver.displayBitmap(Glitch1, 4, 2, true, true);
				myDriver.displayBitmap(Glitch1, 4, 8, false, true);
			}

			break;

		default:
			(*this->emote)->drawMaw();
			break;
	}

	myDriver.display();  // Display the updated maw on the displays

	if (glitchStep < GLITCH_NUM_STEPS) {  // If the animation isn't done yet, increment timer and reschedule
		int32_t nextCycleTime = (((uint32_t) rand() & 0xFFFF) * 4) + GLITCH_BASE_TIME;
      nextStepTime = make_timeout_time_us(nextCycleTime);
      glitchStep++;
		myLogger.logTrace("Next time: %u\n", nextCycleTime);
	} else {  // Otherwise, set that we're stopped, clear the timer, and don't reschedule
		this->animRunning = false;
		this->animScheduled = false;
      glitchStep = 0;
	}
}