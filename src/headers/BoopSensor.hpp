#ifndef __BOOP_H
#define __BOOP_H

#include "APDS9960.hpp"
#include "HiFiProtogenPinout.hpp"
#include "Logger.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "stdint.h"

class BoopSensor {
  private:
	float luxEMA = 0.0f;
	float distEMA = 0.0f;

	const float luxEMAFactor = 0.005;
	const float distEMAFactor = 0.1;

	float boopThreshold = 40.0;

	bool visorOn = false;

	APDS9960 sensor;
	bool exists = false;

  public:
	BoopSensor();
	~BoopSensor();

   bool begin();

	void update();
	bool isBooped();
	bool isVisorOn();
	float getBrightness();
};
#endif
