#pragma once

#include <string>
#include <algorithm>

#include "../Runtime.hpp"

namespace brave {


namespace Paths {
  const std::string exePath = []() {
    auto s = Runtime::exePath();
    std::replace(s.begin(), s.end(), '\\', '/');
    return s + "/";
  }();
  const std::string assets   = exePath + "assets/";
  const std::string figures  = assets + "figures/";
  const std::string motions  = assets + "motions/";
  const std::string shaders  = assets + "shaders/";
  const std::string textures = assets + "textures/";
  const std::string settings = assets + "settings.json";
} // namespace Paths


namespace Figures {
  const std::string plane    = Paths::figures + "plane.obj";
  const std::string sphere   = Paths::figures + "sphere.obj";
  const std::string cube     = Paths::figures + "cube.obj";
  const std::string monkey   = Paths::figures + "monkey.obj";
  const std::string light    = Paths::figures + "light.obj";
  const std::string mark     = Paths::figures + "mark.obj";
  const std::string cylinder = Paths::figures + "cylinder.obj";
} // namespace Figures


namespace Motions {
  const std::string jump = Paths::motions + "jump.bvh";
  const std::string run  = Paths::motions + "run.bvh";
  const std::string walk = Paths::motions + "walk.bvh";
  const std::string idle = Paths::motions + "idle.bvh";
} // namespace Motions


namespace Textures {
  const std::string chess      = Paths::textures + "chess.jpg";
  const std::string metal      = Paths::textures + "metal.jpg";
  const std::string markborder = Paths::textures + "markborder.png";
} // namespace Textures


enum struct loopMode { none, loop, shortLoop };

auto emptyFn = []() {};

} // namespace brave
