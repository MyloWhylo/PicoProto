#include "../headers/ProtoComms.hpp"

bool sendHeartbeat(repeating_timer_t *rt) {
	ProtoComms *thisBadger = (ProtoComms *)(rt->user_data);
	thisBadger->heartbeat();
	return true;
}

ProtoComms::ProtoComms(bool isController, Emotion *emotes, uint numEmotes, Max7219Driver *driver) {
	this->myEmotes = emotes;
	this->numEmotes = numEmotes;
	this->driver = driver;
	this->isController = isController;

	uart_init(uart0, 115200u);
	gpio_set_function(0u, GPIO_FUNC_UART);
	gpio_set_function(1u, GPIO_FUNC_UART);

	uart_set_hw_flow(uart0, false, false);
	uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
}

ProtoComms::~ProtoComms() {}

void ProtoComms::begin() {

	// this->myAlarmPool = alarm_pool_create_with_unused_hardware_alarm(1);
	// alarm_pool_add_repeating_timer_ms(this->myAlarmPool, -500, sendHeartbeat, this, &this->heartbeatTimer);
}

void ProtoComms::heartbeat() {
	// if (this->isController) {
	// 	uint32_t *unused;
	// 	if (!mutex_try_enter(&(this->UARTMutex), unused)) return;

	// 	MessageCommand currentCommand = HEARTBEAT;
	// 	uart_write_blocking(uart0, (const uint8_t *)(&currentCommand), sizeof(MessageCommand));

	// 	mutex_exit(&(this->UARTMutex));
	// } else {
	// 	uint32_t *unused;
	// 	if (!mutex_try_enter(&(this->UARTMutex), unused)) return;

	// 	MessageCommand currentCommand = HEARTBEAT;
	// 	uart_write_blocking(uart0, (const uint8_t *)(&currentCommand), sizeof(MessageCommand));

	// 	mutex_exit(&(this->UARTMutex));
	// }
}

void ProtoComms::sendFaceParams() {
	// mutex_enter_blocking(&(this->UARTMutex));

	// // Send Command
	// MessageCommand currentCommand = OPTION_INFO;
	// uart_write_blocking(uart0, (const uint8_t *)(&currentCommand), sizeof(MessageCommand));

	// // Send Number of Strings
	// uart_write_blocking(uart0, (const uint8_t *)(&this->numEmotes), sizeof(this->numEmotes));

	// // Send Strings
	// for (uint ii = 0; ii < this->numEmotes; ii++) {
	// 	const char *thisString = this->myEmotes[ii].getName().c_str();
	// 	size_t thisLength = strnlen(thisString, 256);
	// 	uart_write_blocking(uart0, (const uint8_t *)thisString, thisLength + 1);
	// }

	// mutex_exit(&(this->UARTMutex));
}

bool ProtoComms::isLinkUp() {
	return this->linkUp;
}