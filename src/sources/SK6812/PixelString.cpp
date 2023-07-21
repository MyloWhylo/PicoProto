#include "../../headers/SK6812.hpp"

// Private function

void PixelString::initializeDMA() {
	dma_channel_config c = dma_channel_get_default_config(this->dmaChannel);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, false);
	channel_config_set_dreq(&c, pio_get_dreq(this->pio, this->sm, true));

	uint transferCnt = this->w;

	dma_channel_configure(dmaChannel, &c,
								 &pio->txf[this->sm],  // Destination pointer
								 this->frontPage,      // Source pointer
								 transferCnt,          // Number of transfers
								 false                 // Start immediately
	);
}

PixelString::PixelString(uint width, bool gamma) {
	this->w = width;
	this->pin = sk6812_PIN;
	this->doGamma = gamma;
	this->frontPage = new Pixel[width];
	this->backPage = new Pixel[width];
}

PixelString::PixelString(uint width, uint pin, bool gamma) {
	this->w = width;
	this->pin = pin;
	this->doGamma = gamma;
	this->frontPage = new Pixel[width];
	this->backPage = new Pixel[width];
}

void PixelString::init() {
	uint offset = pio_add_program(this->pio, &sk6812_program);
	this->dmaChannel = dma_claim_unused_channel(true);
	this->sm = pio_claim_unused_sm(this->pio, true);
	sk6812Init(this->pio, this->sm, offset, PIXEL_CLOCK, pin);
	this->timeBetweenFrames = (this->w * BITS_PER_PIXEL * PIXEL_TIME) + RESET_TIME;
	this->initializeDMA();
}

void PixelString::clear() {
	for (int ii = 0; ii < (this->w); ii++) {
		this->backPage[ii].setPixel(0, 0, 0);
	}
}

Pixel* PixelString::getPixel(uint index) {
	return &(this->backPage[index]);
}

void PixelString::setPixel(uint index, uint8_t r, uint8_t g, uint8_t b) {
	if (this->doGamma) {
		this->backPage[index].setPixel(gamma8(r) * brightness, gamma8(g) * brightness, gamma8(b) * brightness);
	} else {
		this->backPage[index].setPixel(r * brightness, g * brightness, b * brightness);
	}
}

void PixelString::setPixel(uint index, uint32_t rgb) {
	uint8_t r = (rgb & 0xFF0000) >> 16;
	uint8_t g = (rgb & 0x00FF00) >> 8;
	uint8_t b = (rgb & 0x0000FF);

	this->setPixel(index, r, g, b);
}

void PixelString::setPixelRaw(uint index, uint32_t rgbw) {
	this->getPixel(index)->setPixel(rgbw);
}

void PixelString::setBrightness(uint newBright) {
	this->brightness = newBright / 256.0f;
}

void PixelString::display() {
	if (absolute_time_diff_us(get_absolute_time(), this->nextFrameTime) <= 0) {
		sleep_until(nextFrameTime);
	}

	Pixel* tmp = this->backPage;
	this->backPage = this->frontPage;
	this->frontPage = tmp;

	dma_channel_set_read_addr(this->dmaChannel, this->frontPage, true);
	this->nextFrameTime = make_timeout_time_us((uint64_t) this->timeBetweenFrames);
}

bool PixelString::isBusy() {
	return dma_channel_is_busy(this->dmaChannel);
}