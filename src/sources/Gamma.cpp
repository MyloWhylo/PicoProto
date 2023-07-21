#include "../headers/Gamma.hpp"

#include "../headers/Logger.hpp"

extern Logger myLogger;

uint8_t gamma8(uint8_t in) {
	return gamma8Table[in];
}

uint16_t gamma16(uint16_t in) {
	return (uint16_t)(powf((float)in / (float)UINT16_MAX, 2.8f) * UINT16_MAX + 0.5f);
}

uint16_t gamma16f(float in) {
	return (uint16_t)(powf(in, 2.8f) * UINT16_MAX + 0.5f);
}

uint16_t floatToGamma16(float in) {
	if (in > 1.0f) in = 1.0f;
	if (in < 0.0f) in = 0.0f;

	return gamma16f(in);
}

uint8_t floatToGamma8(float in) {
	if (in > 1.0f) in = 1.0f;
	if (in < 0.0f) in = 0.0f;

	uint8_t val = (uint8_t)(in * UINT8_MAX);

	return gamma8(val);
}

uint16_t floatToGamma(float in, uint16_t max_value) {
	if (in > 1.0f) in = 1.0f;
	if (in < 0.0f) in = 0.0f;

	uint16_t retVal = (uint16_t)(powf(in, 2.8f) * max_value + 0.5f);

	myLogger.logTrace("Converting float to gamma. In: %f, max: %d\n", in, max_value);
	myLogger.logTrace("Powf: %f, retVal: %d\n", powf(in, 2.8f), retVal);
	return retVal;
}