#pragma once
#ifndef _BOOPCODE_H
#define _BOOPCODE_H

#include "stdint.h"
#include "stdlib.h"

typedef struct ProtoInfo {
	char* protoName;
	char* protoMaker;
	char* customMsg;
} ProtoInfo;

enum BoopMsgType {
	BOOP_DISCOVERY,
	DATA_REQUEST,
	DATA_REPLY,
	SYNC_START,
	ACK,
	NACK
};

enum BoopState {
	DISCOVERY,
	BACKOFF,
	REQUEST_DATA,
	SEND_DATA,
	RECIEVE_DATA,
	SYNCHRONIZE,
	ERROR
};

typedef struct BoopMSG {
	uint32_t magic_string;
	uint8_t msg_type;
	uint8_t msg_length;
	uint8_t msg_data[247];
} BoopMSG;

class BoopCodeController {
  protected:
	BoopState currentState = DISCOVERY;
	BoopState prevState = DISCOVERY;
	uint8_t currentID = 0;
	ProtoInfo myInfo;

  public:
	BoopCodeController();
	~BoopCodeController();

   virtual void start();
   virtual void stop();

};

#endif
