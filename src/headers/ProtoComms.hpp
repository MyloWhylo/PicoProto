#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Emotion.hpp"
#include "Max7219Driver.hpp"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/mutex.h"


const int8_t CONTINUE           = 3;
const int8_t NEW_DATA           = 2;
const int8_t NO_DATA            = 1;
const int8_t CRC_ERROR          = 0;
const int8_t PAYLOAD_ERROR      = -1;
const int8_t STOP_BYTE_ERROR    = -2;
const int8_t STALE_PACKET_ERROR = -3;

const uint8_t START_BYTE = 0x7E;
const uint8_t STOP_BYTE  = 0x81;

const uint8_t PREAMBLE_SIZE   = 4;
const uint8_t POSTAMBLE_SIZE  = 2;
const uint8_t MAX_PACKET_SIZE = 0xFE; // Maximum allowed payload bytes per packet

const uint8_t DEFAULT_TIMEOUT = 50;

typedef enum {
	ACK,
	NACK,
	HEARTBEAT,
	FACE_CHANGE,
	BRIGHTNESS_CHANGE,
	OPTION_QUERY,
   OPTION_HEADER,
	OPTION_BODY
} MessageCommand;

typedef struct {
	uint8_t startByte = START_BYTE;
	uint8_t packetID;
	uint8_t cobsOverhead;
	uint8_t numBytes;
} MessageHeader;

typedef struct {
	uint8_t crc8;
	uint8_t stopByte = STOP_BYTE;
} MessageFooter;

class ProtoComms {
  private:
	bool linkUp = false;

   Emotion* myEmotes;
   uint numEmotes;
   Max7219Driver* driver;
   bool isController;

	void sendFaceParams();
	void encodeMessage();

  public:
	ProtoComms(bool isController, Emotion* emotes, uint numEmotes, Max7219Driver* driver);
	~ProtoComms();

	void begin();
	void heartbeat();

   bool isLinkUp();
};
