#pragma once

#include <string>
#include <algorithm>

#include <dac/Logger.hpp>
#include <dac/Runtime.hpp>

namespace BRAVE {
namespace Paths {

  const std::string exePath = []() {
    auto s = dac::Runtime::exePath();
    std::replace(s.begin(), s.end(), '\\', '/');
    return s;
  }();

  const std::string Assets = exePath + "/assets/";

  const std::string Bvhs     = Assets + "bvhs/";
  const std::string Shaders  = Assets + "shaders/";
  const std::string Figures  = Assets + "figures/";
  const std::string Textures = Assets + "textures/";
  const std::string Settings = Assets + "settings.json";

} // namespace Paths
} // namespace BRAVE
