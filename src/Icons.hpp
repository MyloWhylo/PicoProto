#pragma once
#ifndef _ICONS_H
#define _ICONS_H

#include <stdio.h>

#include "pico/stdlib.h"
// MYLO'S NOTE:
// I've shamelessly stolen these from Feronium. Hopefully in the future, I'll create
// my own icon set. Unfortunately, I have no art skills whatsoever. I apologize.

// Edit the icons to your liking. The icons are split into a 'left' and a 'right' side.
//  A '0' means the led will be turned off, while a '1' will turn a led on.

/////////////////////////// Facial icons \\\\\\\\\\\\\\\\\\\\\\\\\\\

// Right side of the helmet
static const uint8_t
	 nose[] = {0b00000000,
				  0b01111110,
				  0b00111111,
				  0b00000011,
				  0b00000011,
				  0b00000001,
				  0b00000000,
				  0b00000000};
static const uint8_t
	 maw[] = {0b00100000, 0b00000000, 0b00000000, 0b00000000,
				 0b01111000, 0b00000000, 0b00000000, 0b00000000,
				 0b11011110, 0b00000000, 0b00000000, 0b00000000,
				 0b11000111, 0b10000000, 0b00000000, 0b11100000,
				 0b11111111, 0b11100000, 0b00000111, 0b11111000,
				 0b00000000, 0b01111000, 0b00011110, 0b00011110,
				 0b00000000, 0b00011110, 0b01111000, 0b00000111,
				 0b00000000, 0b00000111, 0b11100000, 0b00000001};
static const uint8_t
	 Glitch1[] = {0b00110000, 0b00010000, 0b00000100, 0b00000100,
					  0b00000000, 0b00101000, 0b01100000, 0b00111010,
					  0b00110001, 0b00101000, 0b00000001, 0b00010011,
					  0b00000111, 0b00100010, 0b00100001, 0b10100001,
					  0b01011101, 0b11110000, 0b00000011, 0b11111111,
					  0b00111000, 0b01101001, 0b01001110, 0b00000100,
					  0b10100000, 0b01001110, 0b01001100, 0b00000110,
					  0b10000001, 0b00000111, 0b11100100, 0b00000000};
static const uint8_t
	 Glitch2[] = {0b00000000, 0b00000000, 0b00000000, 0b00000100,
					  0b00000000, 0b00000000, 0b00000000, 0b00011110,
					  0b00100000, 0b00010000, 0b00000000, 0b01001011,
					  0b00000111, 0b00100000, 0b00000001, 0b11100111,
					  0b00011111, 0b11110000, 0b00000110, 0b11111111,
					  0b00001011, 0b11111101, 0b01111100, 0b00000000,
					  0b11100110, 0b00010110, 0b01011000, 0b00000000,
					  0b00000000, 0b00000111, 0b11100000, 0b00000000};
static const uint8_t
	 eye[] = {0b00001111, 0b00000000,
				 0b00111111, 0b11100000,
				 0b01111111, 0b11111000,
				 0b11111111, 0b11111110,
				 0b11110000, 0b00000111,
				 0b01100000, 0b00000001,
				 0b00000000, 0b00000000,
				 0b00000000, 0b00000000};
static const uint8_t
	 Angry[] = {0b00000000, 0b00000000,
					0b00011111, 0b11111100,
					0b00111111, 0b11111110,
					0b00111111, 0b11111100,
					0b00011111, 0b11111000,
					0b00001111, 0b11100000,
					0b00000011, 0b10000000,
					0b00000000, 0b00000000};
static const uint8_t
	 Spooked[] = {0b00000011, 0b11000000,
					  0b00000111, 0b11100000,
					  0b00001111, 0b11110000,
					  0b00001111, 0b11110000,
					  0b00001111, 0b11110000,
					  0b00001111, 0b11110000,
					  0b00000111, 0b11100000,
					  0b00000011, 0b11000000};
static const uint8_t
	 vwv[] = {0b00001110, 0b00000000,
				 0b00000111, 0b10000000,
				 0b00000001, 0b11100000,
				 0b00000000, 0b01111000,
				 0b00000000, 0b01111000,
				 0b00000001, 0b11100000,
				 0b00000111, 0b10000000,
				 0b00001110, 0b00000000};

#endif