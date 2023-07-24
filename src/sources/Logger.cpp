#include "../headers/Logger.hpp"

Logger::Logger(LogLevel startingLevel) {
	this->currentLevel = startingLevel;
}

Logger::~Logger() {
}

void Logger::log(const char *fmt, ...) {
	if (this->currentLevel < info) return;
	printf("\x1b[39m");

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void Logger::logDebug(const char *fmt, ...) {
#ifndef NDEBUG
	if (this->currentLevel < debug) return;
	printf("\x1b[32m");

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#else
	return;
#endif
}

void Logger::logTrace(const char *fmt, ...) {
#ifndef NDEBUG
	if (this->currentLevel < trace) return;
	printf("\x1b[33m");

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#else
	return;
#endif
}

void Logger::logAbsurd(const char *fmt, ...) {
#ifndef NDEBUG
	if (this->currentLevel < absurd) return;
	printf("\x1b[31m");

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#else
	return;
#endif
}

#ifndef NDEBUG
Logger myLogger(trace);
#else
Logger myLogger(info);
#endif