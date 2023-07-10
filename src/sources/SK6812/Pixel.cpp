#include "../../headers/SK6812.hpp"

extern Logger myLogger;
Pixel::Pixel() {
	this->rgbw = 0x00;
}

Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue) {
	this->setPixel(red, green, blue);
}

void Pixel::setPixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
	myLogger.logTrace("Setting pixel to %d %d %d %d\n", red, green, blue, white);
	this->rgbw = (red << 16) | (green << 24) | (blue << 8) | white;
}

void Pixel::setPixel(uint8_t red, uint8_t green, uint8_t blue) {
	uint8_t white = 0;
	// check to see if red, green, or blue is 0
	// if so, there is no white led used
	if (red == 0 || green == 0 || blue == 0) {
		myLogger.logTrace("Setting pixel to %d %d %d %d\n", red, green, blue, 0);
		this->rgbw = ((red << 16) | (green << 24) | (blue << 8)) & 0xFFFFFF00;
	} else {
		if (red < green) {
			white = blue < red ? blue : red;
		} else {
			white = blue < green ? blue : green;
		}
		red -= white;
		green -= white;
		blue -= white;
		this->setPixel(red, green, blue, white);
	}
}

void Pixel::setPixel(uint32_t hex) {
	this->rgbw = hex;
}
