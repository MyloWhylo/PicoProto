#ifndef _EMOTION_H
#define _EMOTION_H

#include <string>

#include "pico/time.h"
#include "Max7219Driver.hpp"

// enum EmoteName {
// 	Normal,
// 	Suprised,
// 	Glitchy,
// 	Cute
// };

// typedef struct Emotion {
// 	EmoteName name;
// 	uint8_t* eyes;
// 	Max7219Driver* driver;
// } Emotion;

class Emotion {
  private:
	std::string name;
	const uint8_t* eyes;
	const uint8_t* nose;
	const uint8_t* maw;

	uint8_t animTimers[8];
	bool animRunning[8];

  public:
	bool canGlitch;
	bool canBlink;

	Max7219Driver* driver;
	Emotion(std::string emoteName, const uint8_t* eyes, const uint8_t* nose, const uint8_t* maw, bool glitch = false, bool blink = true);
	~Emotion();
	std::string getName();
	void drawAll();
	void drawEyes();
	void drawNose();
	void drawMaw();
};

#endif