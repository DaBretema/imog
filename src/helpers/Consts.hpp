#pragma once

#include <string>
#include <algorithm>

#include <dac/Logger.hpp>
#include <dac/Runtime.hpp>

namespace brave {


namespace Paths {

  const std::string exePath = []() {
    auto s = dac::Runtime::exePath();
    std::replace(s.begin(), s.end(), '\\', '/');
    return s;
  }();

  const std::string assets = exePath + "/assets/";

  const std::string bvhs     = assets + "bvhs/";
  const std::string shaders  = assets + "shaders/";
  const std::string figures  = assets + "figures/";
  const std::string textures = assets + "textures/";
  const std::string settings = assets + "settings.json";

} // namespace Paths


namespace Figures {
  const std::string plane  = Paths::figures + "plane.obj";
  const std::string sphere = Paths::figures + "sphere.obj";
  const std::string cube   = Paths::figures + "cube.obj";
  const std::string monkey = Paths::figures + "monkey.obj";
  const std::string light  = Paths::figures + "light.obj";
  const std::string mark   = Paths::figures + "mark.obj";
} // namespace Figures


namespace Textures {
  const std::string chess      = Paths::textures + "chess.jpg";
  const std::string metal      = Paths::textures + "metal.jpg";
  const std::string markborder = Paths::textures + "markborder.png";
} // namespace Textures


namespace Shaders {
  const std::string base  = "base";
  const std::string light = "light";
} // namespace Shaders

} // namespace brave
