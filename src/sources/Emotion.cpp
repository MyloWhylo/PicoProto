#include "../headers/Emotion.hpp"

#include <string>

#include "../headers/Max7219Driver.hpp"
#include "pico/time.h"

extern Max7219Driver myDriver;  // We unfortunately need the Max7219 driver

Emotion::Emotion(std::string emoteName, const uint8_t* eyes, const uint8_t* nose, const uint8_t* maw, bool glitch, bool blink) {
	this->name = emoteName;
	this->eyes = eyes;
	this->nose = nose;
	this->maw = maw;

	this->canBlink = blink;
	this->canGlitch = glitch;

	this->driver = &myDriver;
}

Emotion::~Emotion() {
}


std::string Emotion::getName() {
	return this->name;
}

// Draws all parts of an emote (eyes, maw, and nose).
void Emotion::drawAll() {
	this->drawEyes();
	this->drawMaw();
	this->drawNose();
}
// Draws the eyes for this emote.
void Emotion::drawEyes() {
	myDriver.displayBitmap(this->eyes, 2, 0, false, true);
	myDriver.displayBitmap(this->eyes, 2, 12, true, true);
}

// Draws the nose for this emote.
void Emotion::drawNose() {
	myDriver.displayBitmap(this->nose, 1, 6, false, true);
	myDriver.displayBitmap(this->nose, 1, 7, true, true);
}

// Draws the maw for this emote.
void Emotion::drawMaw() {
	myDriver.displayBitmap(this->maw, 4, 2, false, true);
	myDriver.displayBitmap(this->maw, 4, 8, true, true);
}