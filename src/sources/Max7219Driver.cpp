#include "../headers/Max7219Driver.hpp"

#include "../headers/Logger.hpp"

extern Logger myLogger;

Max7219Driver::Max7219Driver(uint8_t brightness) {
	this->brightness = (brightness > 15) ? 15 : brightness;
}

Max7219Driver::~Max7219Driver() {
	myLogger.logDebug("Destroying display object...\n");
	this->destroySPI();
}

void Max7219Driver::destroySPI() {
	myLogger.logTrace("Destroying SPI\n");
	if (FACE_SPI_INST == spi0) {
		reset_block(RESETS_RESET_SPI0_BITS);
		unreset_block_wait(RESETS_RESET_SPI0_BITS);
	} else {
		reset_block(RESETS_RESET_SPI1_BITS);
		unreset_block_wait(RESETS_RESET_SPI1_BITS);
	}
}

// -------- I stole these from someone --------
inline void Max7219Driver::setCS() {
	asm volatile("nop \n nop \n nop");  // Delay to meet timing requirements
	gpio_put(FACE_CS, 0);               // Enable CS line
	asm volatile("nop \n nop \n nop");  // Delay to meet timing requirements
}

inline void Max7219Driver::clearCS() {
	asm volatile("nop \n nop \n nop");  // Delay to meet timing requirements
	gpio_put(FACE_CS, 1);               // Disable CS line
	asm volatile("nop \n nop \n nop");  // Delay to meet timing requirements
}

/* \brief Writes the output buffer to the MAX panels.
 */
inline void Max7219Driver::sendData() {
	this->setCS();                                                     // Set CS
	spi_write16_blocking(FACE_SPI_INST, this->outputBuf, NUM_PANELS);  // Send it!
	this->clearCS();                                                   // Clear CS
}

/* \brief Sends a command to every module.
 * \param command Which command to send
 * \param value The value that goes along with that command
 */
void Max7219Driver::setAllDisplays(MaxCommand command, uint8_t value) {
	uint16_t tmp = (command << 8) | value;  // Initialize value to transfer

	for (uint_fast8_t ii = 0; ii < NUM_PANELS; ii++) {  // Fill array
		this->outputBuf[ii] = tmp;
	}

	myLogger.logAbsurd("Sending 0x%02X 0x%02X to all displays\n", (tmp >> 8) & 0xFF, tmp & 0xFF);
	this->sendData();
}

/* \brief Initializes SPI Interface.
 * \return Whether or not it initialized the interface successfully.
 */
bool Max7219Driver::initializeSPI() {
	// Initialize SPI Engine
	myLogger.logTrace("Initializing SPI clock and format\n");
	spi_init(spi0, SPI_BAUDRATE);
	spi_set_format(FACE_SPI_INST, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

	// Initialize SPI TX and SCK pins
	myLogger.logTrace("Initializing GPIO\n");
	gpio_set_function(FACE_SCK, GPIO_FUNC_SPI);
	gpio_set_function(FACE_TX, GPIO_FUNC_SPI);

	// Initialize SPI CS pin
	myLogger.logTrace("Initializing SPI CS pin\n");
	gpio_init(FACE_CS);
	gpio_set_dir(FACE_CS, GPIO_OUT);
	gpio_put(FACE_CS, 1);

	myLogger.logTrace("SPI finished initializing!\n");
	return true;
}

/* \brief Initializes display modules.
 * \return Whether or not it initialized the modules successfully.
 */
bool Max7219Driver::initializeDisplays() {
	bool retVal = true;
	myLogger.logDebug("Initializing Array\n");

	// Clear framebuffer
	// displayArray[ROWS_PER_PANEL][NUM_PANELS * COLS_PER_PANEL]
	for (uint_fast8_t ii = 0; ii < ROWS_PER_PANEL; ii++) {
		for (uint_fast8_t jj = 0; jj < NUM_PANELS * COLS_PER_PANEL; jj++) {
			this->displayArray[ii][jj] = 0x00;
		}
	}

	for (uint_fast8_t ii = 0; ii < NUM_PANELS; ii++) {
		this->outputBuf[ii] = 0x00;
	}

	myLogger.logDebug("Initializing SPI\n");
	retVal &= this->initializeSPI();  // Ensure SPI initializes properly
	this->displaysOn = true;

	myLogger.logDebug("Initializing displays...\n");
	this->setAllDisplays(CMD_SHUTDOWN, 0);                   // Blank the displays
	this->setAllDisplays(CMD_DISPLAYTEST, 0);                // Disable display test
	this->setAllDisplays(CMD_DECODEMODE, 0);                 // Disable BCD decoding
	this->setAllDisplays(CMD_BRIGHTNESS, this->brightness);  // Set display brightness
	this->setAllDisplays(CMD_SCANLIMIT, 7);                  // Enable all rows
	this->display();                                         // Clear the display registers
	this->setAllDisplays(CMD_SHUTDOWN, 1);                   // Re-Enable displays

	
	return retVal;
}

/* \brief Sets a row of a display.
 * \param row Which row you'd like to set
 * \param col Which module to set
 * \param value Bitfield for the LED state
 */
void Max7219Driver::setSegment(uint8_t row, uint8_t col, uint8_t value) {
	this->displayArray[row][col] = value;
}

/* \brief Displays a specified bitmap.
 * \param icon Pointer to the bitmap to display
 * \param width Width of the bitmap, in multiples of bytes.
 * \param panelNum Which panel to display the bitmap on
 * \param flipHorizontal Whether or not to flop the icon horizontally
 * \param flipVertical Whether or not to flip the icon vertically
 */
void Max7219Driver::displayBitmap(const uint8_t* icon, uint8_t width, uint8_t panelNum, bool flipHorizontal, bool flipVertical) {
	if (flipHorizontal) {
		for (uint_fast8_t row = 0; row < 8; row++) {
			for (uint_fast8_t column = 0; column < width; column++) {
				uint32_t currentIcon = __rev(icon[(row * width) + column]) >> 24;  // Bitwise reverse the icon

				if (flipVertical) {
					this->setSegment((7 - row), (((width - 1) + panelNum) - column), currentIcon);
				} else {
					this->setSegment(row, (((width - 1) + panelNum) - column), currentIcon);
				}
			}
		}
	} else {
		for (uint_fast8_t row = 0; row < 8; row++) {
			for (uint_fast8_t column = 0; column < width; column++) {
				uint32_t currentIcon = icon[(row * width) + column];

				if (flipVertical) {
					this->setSegment((7 - row), (panelNum + column), currentIcon);
				} else {
					this->setSegment(row, (panelNum + column), currentIcon);
				}
			}
		}
	}
}

/* \brief Writes entire framebuffer to display.
 */
void Max7219Driver::display() {
	// this->setAllDisplays(CMD_DISPLAYTEST, 0);                // Disable display test
	// this->setAllDisplays(CMD_SCANLIMIT, 7);                  // Enable all rows
	// this->setAllDisplays(CMD_DECODEMODE, 0);                 // Disable BCD decoding
	// this->setAllDisplays(CMD_BRIGHTNESS, this->brightness);  // Set display brightness

	if (!this->displaysOn) return;

	for (uint_fast8_t ii = 0; ii < ROWS_PER_PANEL; ii++) {
		for (uint_fast8_t jj = 0; jj < NUM_PANELS; jj++) {
			this->outputBuf[jj] = ((CMD_DIGIT_START + ii) << 8) | this->displayArray[ii][jj];
		}
		this->sendData();
	}

	// this->setAllDisplays(CMD_SHUTDOWN, 1);  // Re-Enable displays
}

/* \brief Sets the brightness on the displays
 * \param brightness The brightness (0-15) you'd like to set
 */
void Max7219Driver::setBrightness(uint8_t brightness) {
	if (this->brightness == brightness)
		return;
	else {
		this->brightness = brightness;
		this->setAllDisplays(CMD_BRIGHTNESS, this->brightness);
	}
}

/* \brief Gets the current brightness level
 */
uint8_t Max7219Driver::getBrightness() {
	return this->brightness;
}

void Max7219Driver::turnOn() {
	if (this->displaysOn) return;

	this->setAllDisplays(CMD_SHUTDOWN, 1);
	this->displaysOn = true;
}

void Max7219Driver::turnOff() {
	if (!this->displaysOn) return;

	this->setAllDisplays(CMD_SHUTDOWN, 0);
	this->displaysOn = false;
}