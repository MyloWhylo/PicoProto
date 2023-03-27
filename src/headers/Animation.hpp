#pragma once
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "Emotion.hpp"
#include "Max7219Driver.hpp"
#include "pico/time.h"
#include "stdint.h"

class Animation {
  private:
	alarm_callback_t animationDriver;
	alarm_id_t alarmID;

	uint8_t animTimers[8];
	volatile bool animRunning = false;
	volatile bool animScheduled = false;

  public:
	Emotion** emote;
	Animation(alarm_callback_t callback, Emotion** currentEmote);
	~Animation();

	void scheduleAnimation(uint32_t msFromNow);
	void stopAnimation();

	bool isRunning();
	void setRunning(bool status);

	bool isScheduled();

	uint8_t getTimer(uint8_t timer);
	void setTimer(uint8_t timer, uint8_t value);
	void incTimer(uint8_t timer);
	void clearTimer(uint8_t timer);
};

#endif