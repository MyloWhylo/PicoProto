#include "../headers/FanController.hpp"

#include "hardware/gpio.h"
#include "hardware/pwm.h"

FanController::FanController(unsigned int pin, float speed) {
	this->speed = speed;
	this->pin = pin;

	if (speed > 0) {
		this->enabled = true;
	}

	gpio_set_function(pin, GPIO_FUNC_PWM);

	pwm_chan currentChannel = pin % 2 == 0 ? PWM_CHAN_A : PWM_CHAN_B;
	this->sliceNum = pwm_gpio_to_slice_num(pin);
	this->pwmChannel = currentChannel;

	pwm_set_wrap(this->sliceNum, 999);
	pwm_set_clkdiv_int_frac(this->sliceNum, 5, 0);

	uint16_t count = this->speedToCount(speed);
	this->enabled = count > 0 ? true : false;
	pwm_set_chan_level(this->sliceNum, this->pwmChannel, count);
	pwm_set_enabled(this->sliceNum, true);
}

FanController::~FanController() {
	gpio_deinit(this->pin);
}

uint16_t FanController::speedToCount(float speed) {
	if (speed > 1.0f) speed = 1.0f;
	if (speed < 0) speed = 0;

	return (int) ((speed * 990) + 10);
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
