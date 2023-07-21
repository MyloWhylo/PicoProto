#include "../headers/FanController.hpp"

#include "hardware/gpio.h"
#include "hardware/pwm.h"

extern Logger myLogger;

FanController::FanController(float speed) {
	myLogger.logDebug("Initializing Fan Controller\n");

	if (speed > 1.0) speed = 1.0;
	if (speed < 0.0) speed = 0.0;

	this->speed = speed;
	this->enabled = this->speed > 0.0 ? true : false;  // whether or not the fan is spinning

	gpio_set_function(FAN_PWM, GPIO_FUNC_PWM);

	// ***** Initialize PWM Pin*****
	this->pwmChannel = pwm_gpio_to_channel(FAN_PWM);  // Get PWM channel
	this->sliceNum = pwm_gpio_to_slice_num(FAN_PWM);  // Get PWM Slice Number

	uint16_t count = this->speedToCount(this->speed);  // Initial PWM count

	pwm_set_wrap(this->sliceNum, 999);                            // Set PWM to wrap at 999
	pwm_set_clkdiv_int_frac(this->sliceNum, 5, 0);                // Clock PWM at 25 MHz (25,000,000 / 1,000 = 25KHz, the frequency we want)
	pwm_set_chan_level(this->sliceNum, this->pwmChannel, count);  // Set initial trigger level
	pwm_set_enabled(this->sliceNum, true);                        // Enable the PWM

	// ***** Initialize Tachometer Pin*****
	gpio_init(FAN_TACH);            //	Initialize the pin itself
	gpio_set_dir(FAN_TACH, false);  // Set it as input
	gpio_pull_up(FAN_TACH);         // Pull it to VCC (pin is open drain)
}

FanController::~FanController() {
	gpio_deinit(FAN_PWM);
	gpio_deinit(FAN_TACH);
}

uint16_t FanController::speedToCount(float speed) {
	if (speed > 1.0f) speed = 1.0f;
	if (speed < 0) speed = 0;

	return (int)((speed * 990) + 10);
}

void FanController::startFan() {
	if (this->enabled == false) {
		this->enabled == true;
	}

	this->setSpeed(this->speed);
}

void FanController::startFan(float speed) {
	if (this->enabled == false) {
		this->enabled = true;
	}

	this->setSpeed(speed);
}

void FanController::stopFan() {
	if (this->enabled == true) {
		this->enabled == false;
		pwm_set_chan_level(this->sliceNum, this->pwmChannel, this->speedToCount(0));
	}
}

void FanController::setSpeed(float speed) {
	this->speed = speed;
	pwm_set_chan_level(this->sliceNum, this->pwmChannel, this->speedToCount(speed));
}

float FanController::getRPM() {
	myLogger.logDebug("Starting RPM collection\n");
	if (!this->enabled)
		return 0;

	absolute_time_t start;
	while (gpio_get(FAN_TACH) == true) {  // sync step
		tight_loop_contents();
	}
	while (gpio_get(FAN_TACH) == false) {  // sync step
		tight_loop_contents();
	}

	start = get_absolute_time();
	while (gpio_get(FAN_TACH) == true) {  // we're now high, so wait until we fall
		tight_loop_contents();
	}
	while (gpio_get(FAN_TACH) == false) {  // then, wait until we go high
		tight_loop_contents();
	}
	absolute_time_t now = get_absolute_time();

	int64_t delta = absolute_time_diff_us(start, now);

	myLogger.logDebug("Finished collecting RPM. Time delta observed: %ld\n", delta);
	myLogger.logDebug("%f %f\n", 2.0f * delta, (2.0f * delta) / 6e7);

	return 1.0f / ((2.0f * delta) / 6e7);
}
