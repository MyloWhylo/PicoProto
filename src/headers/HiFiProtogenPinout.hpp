#ifndef __HIFIPROTOGEN_H
#define __HIFIPROTOGEN_H

#include "hardware/uart.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

#define BACKPACK_UART_INST uart0
#define BACKPACK_UART_TX 12
#define BACKPACK_UART_RX 13

#define CHEEK_RING_PIN 1

#define BADGER_UART_INST uart1
#define BADGER_UART_TX 8
#define BADGER_UART_RX 9

#define FACE_SPI_INST spi0
#define FACE_CS 4
#define FACE_SCK 2
#define FACE_TX 3

#define BOOP_I2C_INST i2c1
#define BOOP_SDA 6
#define BOOP_SCL 7

#define FAN_TACH 17
#define FAN_PWM 18

#define VBATT_SENSE 26

#endif