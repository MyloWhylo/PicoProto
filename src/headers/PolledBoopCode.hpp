#pragma once
#ifndef _MYLO_BOOPER_H
#define _MYLO_BOOPER_H

#include <vector>

#include "pico/time.h"

#include "BoopCode.hpp"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"

#define BOOPCODE_OUT_PIN 0
#define BOOPCODE_IN_PIN 1

#define BOOPCODE_BAUDRATE 9600
#define BOOPCODE_UART uart0

class PolledBoopCode : public BoopCodeController {
  private:
	char msgBuf[1024];

   uint16_t headPtr = 0;
   uint16_t tailPtr = 0;

   absolute_time_t nextMsgSendTime;

   inline void putBuffer(char in);
   inline char getBuffer();
   inline uint16_t bufferLeft();

   void sendByte();
   void recieveByte();

   void handleDiscovery();

   void sendDiscovery();
   void recieveDiscovery();

  public:
   virtual void start();
   virtual void stop();
	void update();
	void hasMSG();
   BoopMSG getMSG();
};
#endif