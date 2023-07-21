#pragma once
#ifndef __SK6812_H
#define __SK6812_H

#include <stdint.h>
#include <stdio.h>

#include "../sk6812.pio.h"
#include "Gamma.hpp"
#include "Logger.hpp"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#define PIXEL_CLOCK 800000.0f
#define PIXEL_TIME 1.2f
#define BITS_PER_PIXEL 32
#define RESET_TIME 80

// default to pin 2 if the board doesn't have a default sk6812 pin defined
#define sk6812_PIN 2

class Pixel {
  public:
	uint32_t rgbw;

	Pixel();
	Pixel(uint8_t red, uint8_t green, uint8_t blue);
	void setPixel(uint8_t red, uint8_t green, uint8_t blue);
	void setPixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
	void setPixel(uint32_t hex);
};

class PixelString {
  private:
	uint pin;
	Pixel* frontPage;
	Pixel* backPage;

	float timeBetweenFrames;
	absolute_time_t nextFrameTime = nil_time;

	float brightness = 1.0f;
	bool doGamma = true;

	int dmaChannel;
	PIO pio = pio0;
	int sm = 0;

	void initializeDMA();

  public:
	uint w;

	PixelString(uint width, bool gamma);
	PixelString(uint width, uint pin, bool gamma);
	void init();
	void clear();
	Pixel* getPixel(uint index);
	void setPixel(uint index, uint8_t r, uint8_t g, uint8_t b);
	void setPixel(uint index, uint32_t hex);

	void setPixelRaw(uint index, uint32_t hex);
	void setPixelRaw(uint index, uint8_t r, uint8_t g, uint8_t b);

	void setBrightness(uint newBright);
	void display();
	bool isBusy();
};

// I need this array to be contiguous so that the DMA works properly
// ughhugughghguhgughg i hate programming
class PixelMatrix {
  private:
	uint pin;
	Pixel* frontPage;
	Pixel* backPage;
	bool snake;
	uint64_t timeBetweenFrames;
	absolute_time_t nextFrameTime = nil_time;

	float brightness = 0.20f;
	bool doGamma = true;

	int dmaChannel;
	PIO pio = pio0;
	int sm = 0;

	void initializeDMA();

  public:
	uint w;
	uint h;

	PixelMatrix(uint width, uint height, bool snake, bool gamma);
	PixelMatrix(uint width, uint height, bool snake, uint pin, bool gamma);

	void init();
	void clear();

	Pixel* getPixel(uint x, uint y);

	void setPixel(uint index, uint8_t r, uint8_t g, uint8_t b);
	void setPixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b);
	void setPixel(uint x, uint y, uint32_t hex);

	void setBrightness(uint newBright);

	void display();

	bool isBusy();
};
#endif