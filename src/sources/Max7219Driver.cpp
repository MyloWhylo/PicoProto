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
	sleep_us(1);
	gpio_put(FACE_CS, 0);  // Enable CS line
	sleep_us(1);
}

inline void Max7219Driver::clearCS() {
	sleep_us(1);
	gpio_put(FACE_CS, 1);  // Disable CS line
	sleep_us(1);
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

	gpio_set_drive_strength(FACE_CS, SPI_DRIVE_STRENGTH);
	gpio_set_drive_strength(FACE_SCK, SPI_DRIVE_STRENGTH);
	gpio_set_drive_strength(FACE_TX, DATA_DRIVE_STRENGTH);

	gpio_set_slew_rate(FACE_CS, SPI_SLEW_RATE);
	gpio_set_slew_rate(FACE_SCK, SPI_SLEW_RATE);
	gpio_set_slew_rate(FACE_TX, SPI_SLEW_RATE);

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
			this->setSegment(ii, jj, 0);
			this->displayWasUpdated[ii][jj] = true;  // Weird hack: force it to send everything on startup
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
	if (this->displayArray[row][col] != value) {
		this->displayArray[row][col] = value;
		this->displayWasUpdated[row][col] = true;
	}
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

	for (uint_fast8_t row = 0; row < ROWS_PER_PANEL; row++) {
#ifndef FORCE_ALL_PANEL_REFRESH
		bool sendRow = false;

		for (uint_fast8_t col = 0; col < NUM_PANELS; col++) {
			if (this->displayWasUpdated[row][col]) {
				this->outputBuf[col] = ((CMD_DIGIT_START + row) << 8) | this->displayArray[row][col];
				this->displayWasUpdated[row][col] = false;
				sendRow = true;
			} else {
				this->outputBuf[col] = 0;
			}
		}

		if (sendRow) this->sendData();
	}
#else
		for (uint_fast8_t col = 0; col < NUM_PANELS; col++) {
			this->outputBuf[col] = ((CMD_DIGIT_START + row) << 8) | this->displayArray[row][col];
		}
		this->sendData();
	}
#endif
	// this->setAllDisplays(CMD_SHUTDOWN, this->displaysOn);  // Re-Enable displays
}

/* \brief Sets the brightness on the displays
 * \param brightness The brightness (0-15) you'd like to set
 */
void Max7219Driver::setBrightness(uint8_t brightness) {
	if (brightness > 15) brightness = 15;

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

void Max7219Driver::refreshSettings() {
	this->setAllDisplays(CMD_DISPLAYTEST, 0);                // Disable display test
	this->setAllDisplays(CMD_DECODEMODE, 0);                 // Disable BCD decoding
	this->setAllDisplays(CMD_BRIGHTNESS, this->brightness);  // Set display brightness
	this->setAllDisplays(CMD_SCANLIMIT, 7);                  // Enable all rows
}

void Max7219Driver::setPixel(uint8_t x, uint8_t y, bool on) {
	if (x > NUM_PANELS * 8) return;
	if (y > ROWS_PER_PANEL) return;

	uint8_t panel_num = x / 8; // each panel is 8 pixels wide
	uint8_t pixel_num = x % 8;
	uint8_t current_row = this->displayArray[y][panel_num];

	if (on) {
		current_row |= (1 << (7 - pixel_num));
	} else {
		current_row &= ~(1 << (7 - pixel_num));
	}
}


uint8_t Max7219Driver::drawCharacter(char c, uint8_t startX, uint8_t startY) {
	// Make sure it's not below the range
	uint8_t fontFirstChar = Font3x5[FONT_FIRST_CHAR_OFFSET];
	if (c < fontFirstChar) return 0;
	
	c = c - fontFirstChar; // Move it down

	// Make sure it's not above the range
	uint8_t fontCharCount = Font3x5[FONT_CHAR_COUNT_OFFSET];
	if (c > fontCharCount) return 0;


	// Find the starting point for this character
	size_t charStartOffset = FONT_CHAR_WIDTHS_OFFSET + fontCharCount;
	for (size_t ii = 0; ii < c; ii++) {
		charStartOffset += Font3x5[FONT_CHAR_WIDTHS_OFFSET + ii];
	}


	// Get width of character
	uint8_t charWidth = Font3x5[FONT_CHAR_WIDTHS_OFFSET + c];

	for (size_t xx = 0; xx < charWidth; xx++) {
		uint8_t thisColumn = Font3x5[charStartOffset + xx];

		for (size_t yy = 0; yy < 8; yy++) {
			bool on = (thisColumn & (1 << yy)) != 0;
			this->setPixel(startX + xx, startY + yy, on);
		}
	}

	return charWidth;
}

void Max7219Driver::printf(uint8_t starting_panel, const char *fmt, ...) {
	char textBuf[NUM_PANELS * 2];

	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(textBuf, sizeof(char) * (NUM_PANELS * 2), fmt, args);

	uint8_t currentXPos = starting_panel * 8;
	for (int ii = 0; ii < len; ii++) {
		uint8_t charWidth = this->drawCharacter(textBuf[ii], currentXPos, 0);
		currentXPos += charWidth;
		for (int yy = 0; yy < 8; yy++) {
			this->setPixel(currentXPos, yy, false);
		}
		currentXPos++;
	}
}