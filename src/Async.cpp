#include "Async.hpp"
#include "Logger.hpp"

namespace brave {


// ====================================================================== //
// ====================================================================== //
// Runs a function fn each wait time until flag is false;
// ====================================================================== //

void Async::_periodic(const floatFn& timeFn,
                      bool*          threadFlag,
                      const asyncFn& func) {
  try {
    while (*threadFlag) {
      try {
        func();
      } catch (std::exception&) { LOGD("Fail on thread func execution.") }
      std::this_thread::sleep_for(std::chrono::duration<float>(timeFn()));
    }
    LOGD("Pointedly die.");
  } catch (std::exception&) { LOGD("Death by exception."); }
}

// ====================================================================== //
// ====================================================================== //
// Periodic function wrapper that runs it in a separated thead
// ====================================================================== //

void Async::periodic(const floatFn& timeFn,
                     bool*          threadFlag,
                     const asyncFn& func) {
  std::thread(_periodic, timeFn, threadFlag, func).detach();
}

} // namespace brave
