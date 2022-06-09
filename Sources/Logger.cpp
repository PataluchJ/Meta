#include <Logger.hpp>

//#ifdef _DEBUG
uint8_t Logger::flag = 0b111;
//#else
//uint8_t Logger::flag = 0b100;
//#endif

void Logger::logInfo(const std::string& message, bool endl) {
	if ((flag & infoFlag) == 0) {
		return;
	}
	std::cout << message << (endl ? "\n" : "");
}
void Logger::logNotCriticalError(const std::string& message, bool endl) {
	if ((flag & notCriticalErrorFlag) == 0) {
		return;
	}
	std::cout << message << (endl ? "\n" : "");
}
void Logger::logCriticalError(const std::string& message, bool endl) {
	if ((flag & criticalErrorFlag) == 0) {
		return;
	}
	std::cout << message << (endl ? "\n" : "");
}
