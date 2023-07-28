#pragma once
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "Emotion.hpp"
#include "Max7219Driver.hpp"
#include "pico/time.h"
#include "stdint.h"

class Animation {
  protected:
	Emotion** emote;
	absolute_time_t scheduledFor = nil_time;

	volatile bool animRunning = false;
	volatile bool animScheduled = false;

	virtual void runAnimUpdate();

  public:

	Animation(Emotion** currentEmote);
	~Animation();

	void scheduleAnimation(uint32_t msFromNow);
	void stopAnimation();

	void update();

	bool isScheduled();
	bool isRunning();
};

#endif