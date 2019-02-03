#include "Core.hpp"

#include "Settings.hpp"


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Define public variables
// ====================================================================== //

bool                    Core::pause;
bool                    Core::threadsLive;
std::shared_ptr<Light>  Core::light;
std::shared_ptr<Camera> Core::camera;

// ====================================================================== //
// ====================================================================== //
// Initialize process, set variables init values and
// define keyboard actions
// ====================================================================== //

void Core::init() {
  static std::once_flag OF_Init;
  try {
    std::call_once(OF_Init, [&]() {
      threadsLive = true;
      pause       = false;
      camera      = std::move(std::make_shared<Camera>(Settings::mainCameraPos,
                                                  Settings::mainCameraSpeed));
      light       = std::move(std::make_shared<Light>(Settings::mainLightPos,
                                                Settings::mainLightColor));
      IO::windowInit();

      // --- KEYBOARD ACTIONS SETUP -------------------------------------- //
      // ----------------------------------------------------------------- //
#define KBAA(key, fn) IO::keyboardAddAction(key, fn);
      // ----------------------------------------------------------------- //
      KBAA(GLFW_KEY_ESCAPE, [&]() { close(); });
      KBAA(GLFW_KEY_P, [&]() { pause = !pause; });
      // ----------------------------------------------------------------- //
      KBAA(GLFW_KEY_F, [&]() {
        static bool __brave_camSpeedFlag;
        (!__brave_camSpeedFlag) ? camera->multSpeed(10.f)
                                : camera->multSpeed(1.0f);
        __brave_camSpeedFlag = !__brave_camSpeedFlag;
      });
      KBAA(GLFW_KEY_W, [&]() { camera->move(CamDir::front); });
      KBAA(GLFW_KEY_S, [&]() { camera->move(CamDir::back); });
      KBAA(GLFW_KEY_A, [&]() { camera->move(CamDir::left); });
      KBAA(GLFW_KEY_D, [&]() { camera->move(CamDir::right); });
      KBAA(GLFW_KEY_Q, [&]() { camera->move(CamDir::up); });
      KBAA(GLFW_KEY_E, [&]() { camera->move(CamDir::down); });
      // ------------------------------------ / KEYBOARD ACTIONS SETUP --- //
    });
  } catch (std::exception&) { dErr("BRAVE was already initialized"); }
}

// ====================================================================== //
// ====================================================================== //
// Close process, pause the sim execution and kill threads launched
// during the engine execution (the threads must be dependents
// of threadsLive flag)
// ====================================================================== //

void Core::close() {
  pause       = true;
  threadsLive = false;
}

// ====================================================================== //
// ====================================================================== //
// If camera and light are defined, calls to shaders update
// and renderables update and draw
// ====================================================================== //

void Core::frame() {

  if (camera == nullptr || light == nullptr) {
    dErr("Camera or Light ar not defined");
    return;
  }

  camera->frame();
  for (const auto& s : BRAVE::Shader::pool) { s->update(); }
  for (const auto& r : BRAVE::Renderable::pool) { r->draw(); }
}

// ====================================================================== //
// ====================================================================== //
// Define desired variables to update from Settings and run
// the window loop with the user defined render function
// ====================================================================== //

void Core::onUpdate(const _IO_FUNC& fn) {
  auto updateFn = [&]() {
    camera->speed(Settings::mainCameraSpeed);
    light->color(Settings::mainLightColor);
  };

  IO::windowLoop(fn, updateFn);
}

} // namespace BRAVE
