#pragma once
#ifndef LOGGER_DEFS
#define LOGGER_DEFS

#include <stdarg.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "tusb.h"
enum LogLevel { none = 0,
					 info = 1,
					 debug = 2,
					 trace = 3,
					 absurd = 4 };

class Logger {
  private:
	LogLevel currentLevel;

  public:
	Logger(LogLevel startingLevel = info);
	~Logger();

	void log(const char *fmt, ...);
	void logDebug(const char *fmt, ...);
	void logTrace(const char *fmt, ...);
	void logAbsurd(const char *fmt, ...);
};
#endif