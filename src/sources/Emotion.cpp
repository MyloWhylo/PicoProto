#include "../headers/Emotion.hpp"

#include <string>

#include "../headers/Max7219Driver.hpp"
#include "pico/time.h"

extern Max7219Driver myDriver;

Emotion::Emotion(std::string emoteName, const uint8_t* eyes, const uint8_t* nose, const uint8_t* maw) {
	this->name = emoteName;
	this->eyes = eyes;
	this->nose = nose;
	this->maw = maw;
	this->driver = &myDriver;

	for (int ii = 0; ii < 8; ii++) {
		this->animRunning[ii] = false;
		this->animTimers[ii] = 0;
	}
}

Emotion::~Emotion() {
}

void Emotion::drawAll() {
	this->drawEyes();
	this->drawMaw();
	this->drawNose();
}

void Emotion::drawEyes() {
	myDriver.displayBitmap(this->eyes, 2, 0, false, true);
	myDriver.displayBitmap(this->eyes, 2, 12, true, true);
}

void Emotion::drawNose() {
	myDriver.displayBitmap(this->nose, 1, 6, false, true);
	myDriver.displayBitmap(this->nose, 1, 7, true, true);
}
void Emotion::drawMaw() {
	myDriver.displayBitmap(this->maw, 4, 2, false, true);
	myDriver.displayBitmap(this->maw, 4, 8, true, true);
}

uint8_t Emotion::getTimer(uint8_t timer) {
	return this->animTimers[timer];
}

void Emotion::setTimer(uint8_t timer, uint8_t value) {
	this->animTimers[timer] = value;
}

void Emotion::incTimer(uint8_t timer) {
	this->animTimers[timer]++;
}

void Emotion::clearTimer(uint8_t timer) {
	this->animTimers[timer] = 0;
}