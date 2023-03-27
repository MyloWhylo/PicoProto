#include "stdint.h"
#include "hardware/pwm.h"

class FanController {
  private:
   float speed = 0.0;
   bool enabled = false;

   unsigned int pin;
   unsigned int sliceNum;
   pwm_chan pwmChannel;

   uint16_t speedToCount(float speed);

  public:
	FanController(unsigned int pin, float speed);
	~FanController();

   void startFan();
   void startFan(float speed);
   void stopFan();
   void setSpeed(float speed);
};