#pragma once
#ifndef _MAXDRIVER_DEFS
#define _MAXDRIVER_DEFS

#include <stdio.h>

#include "HiFiProtogenPinout.hpp"
#include "Logger.hpp"
#include "hardware/gpio.h"
#include "hardware/resets.h"
#include "hardware/spi.h"
#include "pico/bit_ops.h"
#include "pico/stdlib.h"
#include "pico/sync.h"

#define EYE_PANELS 2
#define NOSE_PANELS 1
#define MOUTH_PANELS 4
#define SIDES 2

#define NUM_PANELS (EYE_PANELS + NOSE_PANELS + MOUTH_PANELS) * SIDES

#define SPI_BAUDRATE 10000000

// #define NUM_PANELS 7
#define ROWS_PER_PANEL 8
#define COLS_PER_PANEL 1  // 1 byte is 8 bits

enum MaxCommand { CMD_NOOP = 0,
						CMD_DIGIT_START = 1,
						CMD_DECODEMODE = 9,
						CMD_BRIGHTNESS = 10,
						CMD_SCANLIMIT = 11,
						CMD_SHUTDOWN = 12,
						CMD_DISPLAYTEST = 15 };

class Max7219Driver {
  private:
	bool initializeSPI();
	void destroySPI();
	uint8_t brightness;

	inline void setCS();
	inline void clearCS();

	inline void sendData();
	void setAllDisplays(MaxCommand command, uint8_t value);

	uint8_t displayArray[ROWS_PER_PANEL][NUM_PANELS * COLS_PER_PANEL];
	uint16_t outputBuf[NUM_PANELS];

  public:
	Max7219Driver(uint8_t brightness = 8);
	~Max7219Driver();
	bool initializeDisplays();
	void setBrightness(uint8_t brightness);
	uint8_t getBrightness();
	void setSegment(uint8_t row, uint8_t col, uint8_t value);
	void displayBitmap(const uint8_t* icon, uint8_t width, uint8_t panelNum, bool flipHorizontal, bool flipVertical);
	void display();
};

#endif