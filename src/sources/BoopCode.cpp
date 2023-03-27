#include "../headers/BoopCode.hpp"
#include "../headers/Logger.hpp"

extern Logger myLogger;

BoopCodeController::BoopCodeController() {
}

BoopCodeController::~BoopCodeController() {
}

void BoopCodeController::start() {
   myLogger.logDebug("Started boop controller\n");
}

void BoopCodeController::stop() {
   myLogger.logDebug("Stopped boop controller\n");
}