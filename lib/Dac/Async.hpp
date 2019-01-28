#pragma once

#include <functional>

namespace DAC {

class Async {
public:
  // Runs a function fn each wait time until flag is false;
  static void periodic(float sleepTime, bool* threadLiveFlag,
                       const std::function<void(void)>& func);
};

} // namespace DAC
