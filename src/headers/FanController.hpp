#include "HiFiProtogenPinout.hpp"
#include "Logger.hpp"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/sync.h"
#include "pico/time.h"
#include "stdint.h"

class FanController {
  private:
	float speed = 0.0;
	bool enabled = false;

	unsigned int pin;
	uint sliceNum;
	uint pwmChannel;

	uint16_t speedToCount(float speed);

  public:
	FanController(float speed);
	~FanController();

	void startFan();
	void startFan(float speed);
	void stopFan();
	void setSpeed(float speed);

	float getRPM();
};