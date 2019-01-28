#pragma once

#include <mutex>

#include "IO.hpp"
#include "Light.hpp"
#include "Camera.hpp"



namespace BRAVE {

class Core {
public:
  static bool pause;
  static bool threadsLive;

  static std::shared_ptr<Light>  light;
  static std::shared_ptr<Camera> camera;

  static void init();
  static void close();

  static void draw(const std::shared_ptr<Renderable>& r, bool grid = false);
  static void onUpdate(const _IO_FUNC& fn);
};

} // namespace BRAVE
