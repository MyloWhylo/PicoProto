#include "../headers/Max7219Driver.hpp"

#include "../headers/Logger.hpp"

extern Logger myLogger;

Max7219Driver::Max7219Driver(uint8_t brightness) {
	this->brightness = (brightness > 15) ? 15 : brightness;
}

Max7219Driver::~Max7219Driver() {
	myLogger.logDebug("Destroying display object...\n");
	myLogger.logTrace("Destroying SPI\n");
	this->destroySPI();
}

void Max7219Driver::destroySPI() {
	reset_block(RESETS_RESET_SPI0_BITS);
	unreset_block_wait(RESETS_RESET_SPI0_BITS);
}

inline void Max7219Driver::setCS() {
	asm volatile("nop \n nop \n nop");      // Delay to meet timing requirements
	gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Enable CS line
	asm volatile("nop \n nop \n nop");      // Delay to meet timing requirements
}

inline void Max7219Driver::clearCS() {
	asm volatile("nop \n nop \n nop");      // Delay to meet timing requirements
	gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);  // Disable CS line
	asm volatile("nop \n nop \n nop");      // Delay to meet timing requirements
}

inline void Max7219Driver::sendData() {
	this->setCS();                                            // Set CS
	spi_write16_blocking(spi0, this->outputBuf, NUM_PANELS);  // send it!
	this->clearCS();                                          // Clear CS
}

void Max7219Driver::setAllDisplays(MaxCommand command, uint8_t value) {
	uint16_t tmp = (command << 8) | value;  // Initialize value to transfer

	for (uint_fast8_t ii = 0; ii < NUM_PANELS; ii++) {  // Fill array
		this->outputBuf[ii] = tmp;
	}

	myLogger.logAbsurd("Sending 0x%02X 0x%02X to all displays\n", (tmp >> 8) & 0xFF, tmp & 0xFF);
	this->sendData();
}

bool Max7219Driver::initializeSPI() {
	// Initialize SPI Engine
	myLogger.logTrace("Initializing SPI clock and format\n");
	spi_init(spi0, 10000000);
	spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

	// Initialize SPI GPIO
	myLogger.logTrace("Initializing GPIO\n");
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

	// Initialize SPI CS pin
	myLogger.logTrace("Initializing SPI CS pin\n");
	gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
	gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

	myLogger.logTrace("SPI finished initializing!\n");
	return true;
}

bool Max7219Driver::initializeDisplays() {
	bool retVal = true;

	myLogger.logDebug("Initializing Array\n");
	for (uint_fast8_t ii = 0; ii < ROWS_PER_PANEL; ii++) {
		for (uint_fast8_t jj = 0; jj < NUM_PANELS * COLS_PER_PANEL; jj++) {
			this->displayArray[ii][jj] = 0;
		}
	}

	for (uint_fast8_t ii = 0; ii < NUM_PANELS; ii++) {
		this->outputBuf[ii] = 0;
	}

	myLogger.logDebug("Initializing SPI\n");
	retVal &= this->initializeSPI();

	myLogger.logDebug("Initializing displays...\n");
	this->setAllDisplays(CMD_SHUTDOWN, 0);
	this->setAllDisplays(CMD_DISPLAYTEST, 0);
	this->setAllDisplays(CMD_SCANLIMIT, 7);
	this->setAllDisplays(CMD_DECODEMODE, 0);
	this->setAllDisplays(CMD_BRIGHTNESS, brightness);
	this->display();
	this->setAllDisplays(CMD_SHUTDOWN, 1);

	return retVal;
}

void Max7219Driver::setSegment(uint8_t row, uint8_t col, uint8_t value) {
	this->displayArray[row][col] = value;
}

void Max7219Driver::displayBitmap(const uint8_t* icon, uint8_t width, uint8_t panelNum, bool flipHorizontal, bool flipVertical) {
	if (flipHorizontal) {
		for (uint_fast8_t row = 0; row < 8; row++) {
			for (uint_fast8_t column = 0; column < width; column++) {
				uint32_t currentIcon = __rev(icon[(row * width) + column]) >> 24;

				if (flipVertical) {
					this->displayArray[7 - row][((width - 1) + panelNum) - column] = currentIcon;
				} else {
					this->displayArray[row][((width - 1) + panelNum) - column] = currentIcon;
				}
			}
		}
	} else {
		for (uint_fast8_t row = 0; row < 8; row++) {
			for (uint_fast8_t column = 0; column < width; column++) {
				uint32_t currentIcon = icon[(row * width) + column];

				if (flipVertical) {
					this->displayArray[7 - row][panelNum + column] = currentIcon;
				} else {
					this->displayArray[row][panelNum + column] = currentIcon;
				}
			}
		}
	}
}

void Max7219Driver::display() {
	for (uint_fast8_t ii = 0; ii < ROWS_PER_PANEL; ii++) {
		for (uint_fast8_t jj = 0; jj < NUM_PANELS; jj++) {
			this->outputBuf[jj] = ((CMD_DIGIT_START + ii) << 8) | this->displayArray[ii][jj];
		}
		this->sendData();
	}
}
