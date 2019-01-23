
#include "../incl/IO.hpp"
#include "../incl/Core.hpp"

namespace BRAVE {

void Core::init() {
  static std::once_flag OF_Init;
  try {
    std::call_once(OF_Init, [&]() {
      pause       = false;
      threadsLive = true;
    });
  } catch (std::exception&) { DacLog_WARN("BRAVE was already initialized"); }
}

void Core::close() {
  pause       = true;
  threadsLive = false;
}

void Core::onUpdate(const _FUNC& fn) { IO::windowLoop(fn); }

} // namespace BRAVE
