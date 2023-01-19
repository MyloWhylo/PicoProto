#include "headers/Animation.hpp"
#include "Icons.hpp"
#include "headers/Max7219Driver.hpp"
#include "headers/Emotion.hpp"

#define blinkTime -50000

volatile int blinkStep = 0;
volatile bool blinkScheduled = false;

//TODO: create Emotion class that has all the data of the struct, but also has functions to display and deal with each emotion.

int64_t blinkAnim(alarm_id_t id, void* user_data) {

	Emotion* emote = *((Emotion**) user_data);
	printf("Running! ID: %d, Step: %d\n", id, blinkStep);

	if (blinkStep < 8) {
		emote->drawEyes();

		for (int ii = 0; ii < blinkStep + 1; ii++) {
			emote->driver->setSegment(7 - ii, 0, 0);
			emote->driver->setSegment(7 - ii, 1, 0);
			emote->driver->setSegment(7 - ii, 12, 0);
			emote->driver->setSegment(7 - ii, 13, 0);
		}
	} else if (blinkStep == 8) {

	} else if (blinkStep > 8) {
		emote->drawEyes();

		for (int ii = 7; ii > (blinkStep - 9); ii--) {
			emote->driver->setSegment(ii, 0, 0);
			emote->driver->setSegment(ii, 1, 0);
			emote->driver->setSegment(ii, 12, 0);
			emote->driver->setSegment(ii, 13, 0);
		}
	}

	emote->driver->display();

	if (blinkStep != 17) {
		blinkStep++;
		return blinkTime;
	} else {
		blinkStep = 0;
		blinkScheduled = false;
		return 0;
	}
}