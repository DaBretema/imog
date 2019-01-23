#pragma once

#include "Light.hpp"
#include "Camera.hpp"
#include "wrap/Global.hpp"

#include <mutex>


namespace BRAVE {
namespace Core {

  bool pause;
  bool threadsLive;

  std::shared_ptr<Light>  light;
  std::shared_ptr<Camera> camera;


  void init();
  void close();
  void onUpdate(const _FUNC& fn);

} // namespace Core
} // namespace BRAVE
