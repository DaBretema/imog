#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace brave {

typedef std::shared_ptr<spdlog::logger> _LogType;

class Logger {

#define PATTERN_PRINT "%v"
#define PATTERN_ALERT "%t.%@ : %^%v%$"

private:
  // Patterns
  static const char *m_patternPrints;
  static const char *m_patternAlerts;

  // Loggers
  static _LogType m_info;
  static _LogType m_error;
  static _LogType m_print;

public:
  // Getter of info logger
  static _LogType &info();
  // Getter of error logger
  static _LogType &error();
  // Getter of print logger
  static _LogType &print();
};

} // namespace brave

#define LOG(...) SPDLOG_LOGGER_INFO(brave::Logger::print(), __VA_ARGS__);
#define LOGD(...) SPDLOG_LOGGER_INFO(brave::Logger::info(), __VA_ARGS__);
#define LOGE(...) SPDLOG_LOGGER_ERROR(brave::Logger::error(), __VA_ARGS__);
