#include "../headers/Animation.hpp"

#include "../headers/Emotion.hpp"

extern Logger myLogger;
extern Max7219Driver myDriver;

/* \brief Creates a new animation object.
 * \param callback The function that drives the animation
 * \param currentEmote Pointer to the current emote
 */
Animation::Animation(alarm_callback_t callback, Emotion** currentEmote) {
	myLogger.logTrace("Constructing animation");
	this->animationDriver = callback;
	this->emote = currentEmote;

	this->animRunning = false;

	for (int ii = 0; ii < 8; ii++) {
		this->clearTimer(ii);
	}
}

Animation::~Animation() {
	if (this->animRunning) this->stopAnimation();
}

/* \brief Starts an animation after a delay.
 * \param msFromNow How many milliseconds in the future to schedule the animation for
 */
void Animation::scheduleAnimation(uint32_t msFromNow) {
	this->alarmID = add_alarm_in_ms(msFromNow, this->animationDriver, this, true);
	this->animScheduled = true;
	myLogger.logTrace("Starting animation. Scheduled for %d ms from now, and has ID %d.\n", msFromNow, this->alarmID);
}

/* \brief Stops the current animation.
 */
void Animation::stopAnimation() {
	if (this->isRunning()) {
		cancel_alarm(this->alarmID);
		this->alarmID = 0;
	}
}

/* \brief Returns the value of a timer.
 * \param timer The index of the timer to get
 * \returns The value stored at that timer index (usually state number)
 */
uint8_t Animation::getTimer(uint8_t timer) {
	return this->animTimers[timer];
}

/* \brief Sets a timer to a specified value.
 * \param timer The index of the timer
 * \param value The value to set the timer to
 */
void Animation::setTimer(uint8_t timer, uint8_t value) {
	this->animTimers[timer] = value;
}

/* \brief Increments a timer.
 * \param timer The index of the timer
 */
void Animation::incTimer(uint8_t timer) {
	this->animTimers[timer]++;
}

/* \brief Clears a timer.
 * \param timer The index of the timer
 */
void Animation::clearTimer(uint8_t timer) {
	this->animTimers[timer] = 0;
}

/* \brief Check if an animation is currently running.
 * \return Whether the animation is running or not
 */
bool Animation::isRunning() {
	return this->animRunning;
}

/* \brief Sets the running status of the animation
 * \param status Whether the animation is running or not
 */
void Animation::setRunning(bool status) {
	this->animRunning = status;
	if (status == false) {
		this->animScheduled = false;
	}
}

bool Animation::isScheduled() {
	return this->animScheduled;
}