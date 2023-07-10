#include "../../headers/SK6812.hpp"

void PixelMatrix::initializeDMA() {
	dma_channel_config c = dma_channel_get_default_config(this->dmaChannel);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, false);
	channel_config_set_dreq(&c, pio_get_dreq(this->pio, this->sm, true));

	uint transferCnt = this->w * this->h;

	dma_channel_configure(dmaChannel, &c,
								 &pio->txf[this->sm],  // Destination pointer
								 this->frontPage,      // Source pointer
								 transferCnt,          // Number of transfers
								 false                 // Start immediately
	);
}

PixelMatrix::PixelMatrix(uint width, uint height, bool snake, bool gamma) {
	this->w = width;
	this->h = height;
	this->snake = snake;
	this->pin = sk6812_PIN;
	this->doGamma = gamma;
	this->frontPage = new Pixel[height * width];
	this->backPage = new Pixel[height * width];
}

PixelMatrix::PixelMatrix(uint width, uint height, bool snake, uint pin, bool gamma) {
	this->w = width;
	this->h = height;
	this->snake = snake;
	this->pin = pin;
	this->doGamma = gamma;
	this->frontPage = new Pixel[height * width];
	this->backPage = new Pixel[height * width];
}

void PixelMatrix::init() {
	
	uint offset = pio_add_program(this->pio, &sk6812_program);
	this->dmaChannel = dma_claim_unused_channel(true);
	this->sm = pio_claim_unused_sm(this->pio, true);
	sk6812Init(this->pio, this->sm, offset, PIXEL_CLOCK, pin);
	this->timeBetweenFrames = (this->w * this->h * BITS_PER_PIXEL * PIXEL_TIME) + RESET_TIME;
	this->initializeDMA();
}

void PixelMatrix::clear() {
	for (int ii = 0; ii < (this->w * this->h); ii++) {
		this->backPage[ii].setPixel(0, 0, 0);
	}
}

Pixel* PixelMatrix::getPixel(uint x, uint y) {
	if (this->snake) {
		return &(this->backPage[(y * w) + ((y % 2) ? (this->w - 1) - x : x)]);
	} else {
		return &(this->backPage[(y * w) + x]);
	}
}

void PixelMatrix::setPixel(uint index, uint8_t r, uint8_t g, uint8_t b) {
	if (this->doGamma) {
		this->backPage[index].setPixel(gamma8(r) * brightness, gamma8(g) * brightness, gamma8(b) * brightness);
	} else {
		this->backPage[index].setPixel(r * brightness, g * brightness, b * brightness);
	}
}

void PixelMatrix::setPixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b) {
	if (this->doGamma) {
		this->getPixel(x, y)->setPixel(gamma8(r) * brightness, gamma8(g) * brightness, gamma8(b) * brightness);
	} else {
		this->getPixel(x, y)->setPixel(r * brightness, g * brightness, b * brightness);
	}
}

void PixelMatrix::setPixel(uint x, uint y, uint32_t hex) {
	this->getPixel(x, y)->setPixel(hex);
}

void PixelMatrix::setBrightness(uint newBright) {
	this->brightness = newBright / 256.0f;
}

void PixelMatrix::display() {
	if (absolute_time_diff_us(get_absolute_time(), this->nextFrameTime) <= 0) {
		sleep_until(nextFrameTime);
	}

	Pixel* tmp = this->backPage;
	this->backPage = this->frontPage;
	this->frontPage = tmp;

	dma_channel_set_read_addr(this->dmaChannel, this->frontPage, true);
	this->nextFrameTime = make_timeout_time_us(this->timeBetweenFrames);
}

bool PixelMatrix::isBusy() {
	return dma_channel_is_busy(this->dmaChannel);
}