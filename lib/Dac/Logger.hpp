#pragma once

#include <mutex>
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace dac {

class Logger {

private:
  // Initialization
  static void           init();
  static std::once_flag m_once_init;

  // Patterns
  static const char* m_patternPrints;
  static const char* m_patternAlerts;

  // Loggers
  static std::shared_ptr<spdlog::logger> m_info;
  static std::shared_ptr<spdlog::logger> m_error;
  static std::shared_ptr<spdlog::logger> m_print;


public:
  // Detailed print mehtod, w/ thread, line and file data
  // (NOTE: Output goes to stdout)
  template <typename... Args>
  static inline void info(const Args&... args) {
    std::call_once(m_once_init, init);
    SPDLOG_LOGGER_INFO(m_info, args...);
  }

  // Error alert method, w/ thread, line and file data
  // (NOTE: Output goes to stderr)
  template <typename... Args>
  static inline void err(const Args&... args) {
    std::call_once(m_once_init, init);
    SPDLOG_LOGGER_ERROR(m_error, args...);
  }

  // Print mehtod (NOTE: Output goes to stdout)
  template <typename... Args>
  static inline void print(const Args&... args) {
    std::call_once(m_once_init, init);
    SPDLOG_LOGGER_INFO(m_print, args...);
  }
};

} // namespace dac

using dlog = dac::Logger;
