#include "BlinkAnim.hpp"

extern Logger myLogger;
extern Max7219Driver myDriver;

void BlinkAnimation::runAnimUpdate() {
	static int_fast8_t blinkStep = -1;

	int64_t timeDelta = absolute_time_diff_us(this->scheduledFor, get_absolute_time());

	int64_t calcStep = timeDelta / BLINK_CYCLE_TIME;
	if (calcStep == blinkStep)
		return;
	else
		blinkStep = calcStep;

	if (blinkStep == 0) this->animRunning = true;  // If this is the first step, set that we're running

	if (blinkStep < 8) {            // Draw the blink going downward
		(*this->emote)->drawEyes();  // Drawing the eyes here allows us to switch animations while blinking

		for (int ii = 0; ii < blinkStep + 1; ii++) {  // Blank the rows in the framebuffer
			myDriver.setSegment(7 - ii, 0, 0);
			myDriver.setSegment(7 - ii, 1, 0);
			myDriver.setSegment(7 - ii, 12, 0);
			myDriver.setSegment(7 - ii, 13, 0);
		}
	} else if (blinkStep == 8) {
		return;                      // Wait a cycle
	} else if (blinkStep > 8) {     // Draw the blink going upward
		(*this->emote)->drawEyes();  // Drawing the eyes here allows us to switch animations while blinking

		for (int ii = 7; ii > (blinkStep - 9); ii--) {  // Blank the rows in the framebuffer
			myDriver.setSegment(ii, 0, 0);
			myDriver.setSegment(ii, 1, 0);
			myDriver.setSegment(ii, 12, 0);
			myDriver.setSegment(ii, 13, 0);
		}
	}

	myDriver.display();  // Display the updated eyes on the displays

	if (blinkStep >= 17) {  // If we're done, set that we're stopped, clear the timer, and don't reschedule
		this->animRunning = false;
		this->animScheduled = false;
		blinkStep = -1;
	}
}