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

	const float luxEMAFactor = 0.1;
	const float distEMAFactor = 0.1;

	APDS9960 sensor;
	bool exists = false;

  public:
	BoopSensor();
	~BoopSensor();

   bool begin();

	void update();
	bool isBooped();
	uint8_t getBrightness();
};
