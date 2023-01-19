#include "../headers/Animation.hpp"
#include "../headers/Emotion.hpp"

extern Logger myLogger;

Animation::Animation(alarm_callback_t callback, Emotion** currentEmote) {
	myLogger.logTrace("Constructing animation");
	this->animationDriver = callback;
	this->emote = currentEmote;
}

Animation::~Animation() {
}

void Animation::startAnimation(uint32_t msFromNow) {
	this->alarmID = add_alarm_in_ms(msFromNow, this->animationDriver, this->emote, true);
	myLogger.logTrace("Starting animation. Scheduled for %d ms from now, and has ID %d.\n", msFromNow, this->alarmID);
}

void Animation::stopAnimation() {
	cancel_alarm(this->alarmID);
	this->alarmID = 0;
}