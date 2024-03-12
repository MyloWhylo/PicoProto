#ifndef __BOOP_H
#define __BOOP_H

#include "APDS9960.hpp"
#include "HiFiProtogenPinout.hpp"
#include "Logger.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "stdint.h"
#include "MinFilter.hpp"

#define BOOP_SAMPLE_INTERVAL 200 * 1000
// #define ENABLE_VISOR_DETECTION

class BoopSensor {
  private:
	float luxEMA = 0.0f;
	float distEMA = 0.0f;

	const float luxEMAFactor = 0.75;
	const float distEMAFactor = 0.9;

	MinFilter<10, uint8_t> boopFilter;

	bool booped = false;

	int boopThreshold = 5;

	bool visorOn = false;

	absolute_time_t nextSampleTime;

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
