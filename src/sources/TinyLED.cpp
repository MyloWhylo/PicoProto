#include "../headers/TinyLED.hpp"

extern Logger myLogger;

TinyLED::TinyLED(bool PWM) {
	this->doPWM = PWM;
	myLogger.logDebug("Initializing status LED\n");

	for (int ii = 0; ii < 3; ii++) {
		if (PWM) {
			myLogger.logTrace("Pin %d, setting PWM mode.\n", this->pins[ii]);

			uint slice_num = pwm_gpio_to_slice_num(this->pins[ii]);
			uint chan = pwm_gpio_to_channel(this->pins[ii]);
			pwm_set_wrap(slice_num, COUNTER_TOP);

			pwm_set_chan_level(slice_num, chan, COUNTER_TOP * PICO_DEFAULT_LED_PIN_INVERTED);
			pwm_set_enabled(slice_num, true);
			gpio_set_function(this->pins[ii], GPIO_FUNC_PWM);
		} else {
			myLogger.logTrace("Pin %d, setting GPIO mode.\n", this->pins[ii]);
			gpio_init(this->pins[ii]);
			gpio_put(this->pins[ii], 1 * PICO_DEFAULT_LED_PIN_INVERTED);
		}

		gpio_set_dir(this->pins[ii], true);
		gpio_set_drive_strength(this->pins[ii], GPIO_DRIVE_STRENGTH_12MA);
	}
}

TinyLED::~TinyLED() {
	myLogger.logDebug("Destroying TinyLED object.\n");
	if (this->doPWM) {
		this->disablePWM();
	}

	for (int ii = 0; ii < 3; ii++) {
		gpio_deinit(this->pins[ii]);
	}
}

void TinyLED::enablePWM() {
	myLogger.logDebug("Enabling PWM on status LED\n");
	if (!this->doPWM) {
		for (int ii = 0; ii < 3; ii++) {
			uint slice_num = pwm_gpio_to_slice_num(this->pins[ii]);
			uint chan = pwm_gpio_to_channel(this->pins[ii]);
			pwm_set_wrap(slice_num, COUNTER_TOP);

			pwm_set_chan_level(slice_num, chan, COUNTER_TOP * PICO_DEFAULT_LED_PIN_INVERTED);
			pwm_set_enabled(slice_num, true);
			gpio_set_function(this->pins[ii], GPIO_FUNC_PWM);
		}
	}

	this->doPWM = true;
}

void TinyLED::disablePWM() {
	myLogger.logDebug("Disabling PWM on status LED\n");
	if (this->doPWM) {
		for (int ii = 0; ii < 3; ii++) {
			gpio_set_function(this->pins[ii], GPIO_FUNC_SIO);
		}
	}

	this->doPWM = false;
}

void TinyLED::setColor(float r, float g, float b) {
	myLogger.logDebug("Setting color of status LED\n");
	if (this->doPWM) {
		uint slice_num = pwm_gpio_to_slice_num(this->pins[0]);
		uint chan = pwm_gpio_to_channel(this->pins[0]);
		uint16_t setLevel = COUNTER_TOP - floatToGamma(r, COUNTER_TOP);
		pwm_set_chan_level(slice_num, chan, setLevel);
		myLogger.logTrace("Red LED: Slice %d, Channel %d, Level: %d\n", slice_num, chan, setLevel);

		slice_num = pwm_gpio_to_slice_num(this->pins[1]);
		chan = pwm_gpio_to_channel(this->pins[1]);
		setLevel = COUNTER_TOP - floatToGamma(g, COUNTER_TOP);
		pwm_set_chan_level(slice_num, chan, setLevel);
		myLogger.logTrace("Green LED: Slice %d, Channel %d, Level: %d\n", slice_num, chan, setLevel);

		slice_num = pwm_gpio_to_slice_num(this->pins[2]);
		chan = pwm_gpio_to_channel(this->pins[2]);
		setLevel = COUNTER_TOP - floatToGamma(b, COUNTER_TOP);
		pwm_set_chan_level(slice_num, chan, setLevel);
		myLogger.logTrace("Blue LED: Slice %d, Channel %d, Level: %d\n", slice_num, chan, setLevel);
	} else {
		gpio_put(this->pins[0], r >= 0.5f ? 0 : 1);
		gpio_put(this->pins[1], g >= 0.5f ? 0 : 1);
		gpio_put(this->pins[2], b >= 0.5f ? 0 : 1);
		myLogger.logTrace("R: %d, G: %d, B: %d\n", (r >= 0.5f ? 1 : 0), (g >= 0.5f ? 1 : 0), (b >= 0.5f ? 1 : 0));
	}
}