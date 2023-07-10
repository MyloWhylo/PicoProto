#include <stdio.h>

#include "Gamma.hpp"
#include "Logger.hpp"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define COUNTER_TOP 4095

class TinyLED {
  private:
	bool doPWM = false;
	uint pins[3] = {TINY2040_LED_R_PIN, TINY2040_LED_G_PIN, TINY2040_LED_B_PIN};

  public:
	TinyLED(bool PWM = false);
	void enablePWM();
	void disablePWM();
	void setColor(float r, float g, float b);
	~TinyLED();
};
