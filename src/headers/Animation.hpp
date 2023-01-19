#pragma once
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "Logger.hpp"
#include "Emotion.hpp"
#include "Max7219Driver.hpp"
#include "pico/time.h"
#include "stdint.h"

class Animation {
  private:
	alarm_callback_t animationDriver;
	alarm_id_t alarmID;
	Emotion** emote;

  public:
	Animation(alarm_callback_t callback, Emotion** currentEmote);
	~Animation();

	void startAnimation(uint32_t msFromNow);
	void stopAnimation();
};

#endif