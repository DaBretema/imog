#pragma once

#include <mutex>

#include "IO.hpp"
#include "Light.hpp"
#include "Camera.hpp"



namespace BRAVE {

class Core {
public:
  // Declare public variables
  static bool                    pause;
  static bool                    threadsLive;
  static std::shared_ptr<Light>  light;
  static std::shared_ptr<Camera> camera;

  // Initialize process, set variables init values and
  // define keyboard actions
  static void init();

  // Close process, pause the sim execution and kill threads launched
  // during the engine execution (the threads must be dependents
  // of threadsLive flag)
  static void close();

  // If camera and light are defined, calls to shaders update
  // and renderables update and draw
  static void frame();

  // Define desired variables to update from Settings and run
  // the window loop with the user defined render function
  static void onUpdate(const _IO_FUNC& fn);
};

} // namespace BRAVE
