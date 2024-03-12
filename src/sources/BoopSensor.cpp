#include "../headers/BoopSensor.hpp"

extern Logger myLogger;

BoopSensor::BoopSensor() {
	this->nextSampleTime = nil_time;
}

bool BoopSensor::begin() {
	// myLogger.logDebug("Attempting to begin boop sensor\n");
	this->exists = this->sensor.begin(10, APDS9960_AGAIN_4X, APDS9960_ADDRESS, BOOP_I2C_INST, BOOP_SCL, BOOP_SDA);
	// if (this->exists)
	// 	myLogger.logDebug("Boop Sensor Detected!\n");
	// else
	// 	myLogger.logDebug("Boop Sensor does not exist!\n");

	if (!this->exists) return false;

	// myLogger.logDebug("Boop sensor initialized!\n");

	this->sensor.enableProximity();
	this->sensor.enableColor();

	uint16_t r, g, b, c;
	while (!this->sensor.colorDataReady()) {
	}

	this->sensor.getColorData(&r, &g, &b, &c);
	this->luxEMA = (float)this->sensor.calculateLux(r, g, b);
	this->distEMA = (float)this->sensor.readProximity();

	this->nextSampleTime = make_timeout_time_us(BOOP_SAMPLE_INTERVAL);

	return true;
}

BoopSensor::~BoopSensor() {
	if (this->exists) this->sensor.~APDS9960();
}

void BoopSensor::update() {
	if (absolute_time_diff_us(this->nextSampleTime, get_absolute_time()) < 0) {
		return;
	}

	this->nextSampleTime = delayed_by_us(this->nextSampleTime, BOOP_SAMPLE_INTERVAL);

	if (this->exists) {
		if (this->sensor.colorDataReady()) {
			uint16_t r, g, b, c;
			this->sensor.getColorData(&r, &g, &b, &c);
			myLogger.logDebug("r: %u, g: %u, b: %u, c: %u\n", r, g, b, c);
			this->luxEMA = ((1.0f - this->luxEMAFactor) * this->luxEMA) + ((this->luxEMAFactor) * ((float)c));
		}

		uint8_t proximity = this->sensor.readProximity();
		uint8_t minimum = boopFilter.Filter(proximity);

		this->booped = (proximity > minimum + boopThreshold);

		myLogger.logDebug("prox: %u\n", proximity);
		this->distEMA = ((1.0f - this->distEMAFactor) * this->distEMA) + (this->distEMAFactor * ((float)proximity));
		myLogger.logDebug("lux: %f, boop: %f\n", luxEMA, distEMA);
	}
}

bool BoopSensor::isBooped() {
	if (this->exists) return this->booped;
	return false;
	// return this->distEMA >= this->boopThreshold;
}

bool BoopSensor::isVisorOn() {
#ifdef ENABLE_VISOR_DETECTION
	if (this->exists) return this->distEMA >= 10.0f;
#endif
	return true;
}

float BoopSensor::getBrightness() {
	return this->luxEMA;
}