#ifndef __HIFIPROTOGEN_H
#define __HIFIPROTOGEN_H

#include "hardware/uart.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

#define BACKPACK_UART_INST uart0
#define BACKPACK_UART_TX 0
#define BACKPACK_UART_RX 1

#define CHEEK_RING_PIN 2

#define BADGER_UART_INST uart1
#define BADGER_UART_TX 4
#define BADGER_UART_RX 5

#define FACE_SPI_INST spi0
#define FACE_CS 3
#define FACE_SCK 6
#define FACE_TX 7

#define BOOP_I2C_INST i2c1
#define BOOP_SDA 26
#define BOOP_SCL 27

#define FAN_TACH 29
#define FAN_PWM 28

#endif