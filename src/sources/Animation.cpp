#include "../headers/Animation.hpp"

#include "../headers/Emotion.hpp"

extern Logger myLogger;
extern Max7219Driver myDriver;

/* \brief Creates a new animation object.
 * \param callback The function that drives the animation
 * \param currentEmote Pointer to the current emote
 */
Animation::Animation(Emotion** currentEmote) {
	myLogger.logTrace("Constructing animation");
	this->emote = currentEmote;

	this->animRunning = false;
}

Animation::~Animation() {
	if (this->animRunning) this->stopAnimation();
}

void Animation::runAnimUpdate(){
	return;
}

/* \brief Starts an animation after a delay.
 * \param msFromNow How many milliseconds in the future to schedule the animation for
 */
void Animation::scheduleAnimation(uint32_t msFromNow) {
	this->scheduledFor = make_timeout_time_ms(msFromNow);
	this->animScheduled = true;
	myLogger.logTrace("Starting animation. Scheduled for %d ms from now.\n", msFromNow);
}

/* \brief Stops the current animation.
 */
void Animation::stopAnimation() {
	this->animRunning = false;
	this->animScheduled = false;
}

/* \brief Check if an animation is currently running.
 * \return Whether the animation is running or not
 */
bool Animation::isRunning() {
	return this->animRunning;
}

/* \brief Check if an animation is scheduled to run. Note that the scheduled status only resets once the animation is finished.
 * \return Whether the animation is scheduled
 */
bool Animation::isScheduled() {
	return this->animScheduled;
}

void Animation::update() {
	if (!this->animScheduled) return;
	if (absolute_time_diff_us(this->scheduledFor, get_absolute_time()) < 0) return;
	this->runAnimUpdate();
}