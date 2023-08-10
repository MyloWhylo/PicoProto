#include "../headers/BoopSensor.hpp"

extern Logger myLogger;

BoopSensor::BoopSensor() {
}

bool BoopSensor::begin() {
   this->exists = this->sensor.begin(10, APDS9960_AGAIN_4X, APDS9960_ADDRESS, BOOP_I2C_INST, BOOP_SCL, BOOP_SDA);
	if (!this->exists) return false;

   myLogger.logDebug("Boop sensor initialized!\n");

	this->sensor.enableProximity();
	this->sensor.enableColor();

	uint16_t r, g, b, c;
	while (!this->sensor.colorDataReady()) {
	}

	this->sensor.getColorData(&r, &g, &b, &c);
	this->luxEMA = (float)this->sensor.calculateLux(r, g, b);
   this->distEMA = (float)this->sensor.readProximity();
   
   return true;
}

BoopSensor::~BoopSensor() {
	if (this->exists) this->sensor.~APDS9960();
}

void BoopSensor::update() {
   if (this->exists) {
      if (this->sensor.colorDataReady()) {
         uint16_t r, g, b, c;
         this->sensor.getColorData(&r, &g, &b, &c);
         myLogger.logDebug("r: %u, g: %u, b: %u, c: %u\n", r, g, b, c);
         this->luxEMA = ((1.0f - this->luxEMAFactor) * this->luxEMA) + ((this->luxEMAFactor) * ((float) c));
      }

      uint8_t proximity = this->sensor.readProximity();
      myLogger.logDebug("prox: %u\n", proximity);
      this->distEMA = ((1.0f - this->distEMAFactor) * this->distEMA) + (this->distEMAFactor * ((float) proximity));
      myLogger.logDebug("lux: %f, boop: %f\n", luxEMA, distEMA);
   }
}

bool BoopSensor::isBooped() {
   printf("bema: %f\n", this->distEMA);
   if (this->exists) return this->distEMA >= 50.0f;
   else return false;
}

uint8_t BoopSensor::getBrightness() {
   printf("lema: %f\n", this->luxEMA);
   uint32_t brightnessValue = (uint32_t) this->luxEMA;
	return brightnessValue  / 1024;
}