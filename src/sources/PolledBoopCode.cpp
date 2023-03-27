#include "../headers/PolledBoopCode.hpp"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "stdlib.h"
#include "string.h"

void PolledBoopCode::start() {
	gpio_set_function(BOOPCODE_IN_PIN, GPIO_FUNC_UART);
	gpio_set_function(BOOPCODE_OUT_PIN, GPIO_FUNC_UART);

	uart_init(BOOPCODE_UART, BOOPCODE_BAUDRATE);
	uart_set_format(BOOPCODE_UART, 8, 1, UART_PARITY_NONE);
}

void PolledBoopCode::stop() {
	uart_deinit(BOOPCODE_UART);

	gpio_deinit(BOOPCODE_IN_PIN);
	gpio_deinit(BOOPCODE_OUT_PIN);
}

inline void PolledBoopCode::putBuffer(char in) {
	this->msgBuf[this->headPtr] = in;
	this->headPtr++;
	this->headPtr %= 1024;
}

inline char PolledBoopCode::getBuffer() {
	char retChar = this->msgBuf[this->tailPtr];
	this->tailPtr++;
	this->tailPtr %= 1024;

	return retChar;
}

inline uint16_t PolledBoopCode::bufferLeft() {
	return (this->headPtr - this->tailPtr) + (-((int)(this->headPtr <= this->tailPtr)) & 1024);
}

void PolledBoopCode::handleDiscovery() {
	while (this->bufferLeft() >= 5) {
		if (strncmp(&(this->msgBuf[this->tailPtr]), "BOOP", 5) == 0) {
			this->currentState = SEND_DATA;
			this->tailPtr += 5;
			this->tailPtr %= 1024;
			return;
		} else {
			this->tailPtr++;
			this->tailPtr %= 1024;
		}
	}

	if (absolute_time_diff_us(this->nextMsgSendTime, get_absolute_time()) <= 0) {
		this->nextMsgSendTime = make_timeout_time_ms(500);
		this->sendDiscovery();
	}
}

void PolledBoopCode::update() {
	while (uart_is_readable(BOOPCODE_UART)) {
		this->putBuffer(uart_getc(BOOPCODE_UART));
	}

	switch (this->currentState) {
		case DISCOVERY:
			this->handleDiscovery();
			break;

		default:
			break;
	}
}

void PolledBoopCode::hasMSG() {
}

BoopMSG PolledBoopCode::getMSG() {
	return BoopMSG();
}