#ifndef META_LOGGER_H_
#define META_LOGGER_H_

#include <iostream>
#include <string>

class Logger {
public:
	static void logInfo(const std::string& message, bool endl = true);
	static void logNotCriticalError(const std::string& message, bool endl = true);
	static void logCriticalError(const std::string& message, bool endl = true);
		
	static const uint8_t infoFlag = 0b001;
	static const uint8_t notCriticalErrorFlag = 0b010;
	static const uint8_t criticalErrorFlag = 0b100;
private:
	static uint8_t flag;
};

#endif