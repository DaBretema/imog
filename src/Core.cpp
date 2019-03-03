#include "Core.hpp"

#include "Settings.hpp"
#include "helpers/Consts.hpp"

#include "Shader.hpp"
#include "Renderable.hpp"


namespace brave {

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

void Core::init(const std::string settingsPath) {
  static std::once_flag coreInitOnceFlag;
  try {
    std::call_once(coreInitOnceFlag, [&]() {

#define defShaderCreation(name, hasGeom)                    \
  {                                                         \
    std::string sPath = Paths::shaders + name + "/" + name; \
    std::string sV    = sPath + ".vert";                    \
    std::string sG    = (hasGeom) ? sPath + ".geom" : "";   \
    std::string sF    = sPath + ".frag";                    \
    Shader::create(name, sV, sG, sF);                       \
  }

#define KBAA(key, state, fn) IO::keyboardAddAction(key, state, [&]() { fn; });
      //

      // 1. Init settings
      Settings::init(settingsPath);

      // 2. Init GLFW window
      IO::windowInit();

      // 3.1 Create shaders
      defShaderCreation(Shaders::base, true);
      defShaderCreation(Shaders::light, false);

      // 3.2 Crate renderables
      Renderable::create(false, "Monkey", Figures::monkey, "", Colors::orange);
      Renderable::create(false, "Joint", Figures::sphere, "", Colors::orange);
      Renderable::create(false, "Bone", Figures::cylinder, "", Colors::orange);
      Renderable::create(true, "Floor", Figures::plane, Textures::chess);

      // 4. Setup core variables
      threadsLive = true;
      pause       = false;
      camera      = std::make_shared<Camera>(Settings::mainCameraSpeed,
                                        Settings::mainCameraRot.x,
                                        Settings::mainCameraRot.y);
      light       = std::make_shared<Light>(Settings::mainLightPos,
                                      Settings::mainLightColor,
                                      Settings::mainLightIntensity);

      // 5. Define keyboard actions
      auto defaultState = IO::kbState::press_and_repeat;

      KBAA(GLFW_KEY_ESCAPE, defaultState, close());
      KBAA(GLFW_KEY_P, defaultState, pause = !pause);

      // KBAA(GLFW_KEY_Q, defaultState, camera->move(CamDir::up));
      // KBAA(GLFW_KEY_E, defaultState, camera->move(CamDir::down));
      // KBAA(
      //     GLFW_KEY_W,
      //     defaultState,
      //     camera->transform.pos += camera->transform.front() * camera->speed());
      // KBAA(
      //     GLFW_KEY_S,
      //     defaultState,
      //     camera->transform.pos -= camera->transform.front() * camera->speed());
      // KBAA(GLFW_KEY_W, defaultState, camera->move(CamDir::front));
      // KBAA(GLFW_KEY_A, defaultState, camera->move(CamDir::left));
      // KBAA(GLFW_KEY_D, defaultState, camera->move(CamDir::right));

      KBAA(GLFW_KEY_F, defaultState, static bool __brave_camSpeedFlag;
           (!__brave_camSpeedFlag) ? camera->multSpeed(10.f)
                                   : camera->multSpeed(1.0f);
           __brave_camSpeedFlag = !__brave_camSpeedFlag;);
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
  IO::windowOnClose(IO::window());
}

// ====================================================================== //
// ====================================================================== //
// If camera and light are defined, calls to shaders update
// and renderables update and draw
// ====================================================================== //

void Core::frame() {

  if (!camera || !light) {
    dErr("Camera or Light ar not defined");
    return;
  }

  camera->frame();
  for (const auto& s : Shader::pool) { s->update(camera, light); }
  for (const auto& r : Renderable::pool) {
    if (r->allowGlobalDraw()) r->draw(camera);
  }
}

// ====================================================================== //
// ====================================================================== //
// Define desired variables to update from Settings and run
// the window loop with the user defined render function
// ====================================================================== //

void Core::onUpdate(const _IO_FUNC& fn) {
  auto updateFn = [&]() {
    light->pos(Settings::mainLightPos);
    light->color(Settings::mainLightColor);
    light->intensity(Settings::mainLightIntensity);

    camera->speed(Settings::mainCameraSpeed);
    // camera->transform.pos = Settings::mainCameraPos;
  };

  IO::windowLoop(fn, updateFn);
}

} // namespace brave
