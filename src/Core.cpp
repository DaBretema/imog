
#include "../incl/Core.hpp"
#include "../incl/Settings.hpp"

// #define GLFW_INCLUDE_NONE
// #include <glfw/glfw3.h>

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
  } catch (std::exception&) { dlog::err("BRAVE was already initialized"); }
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
// If camera and light are defined, upload its data to the shader
// memory of renderable object, setup if renderable will be drawed
// as solid or grid way and call renderable draw
// ====================================================================== //

void Core::draw(const std::shared_ptr<Renderable>& r, bool grid) {

  if (camera == nullptr || light == nullptr) {
    dlog::err("Camera or Light ar not defined");
    return;
  }

  // --- Upload LIGHT data ------------------------------------------------- //
  // ----------------------------------------------------------------------- //
  r->shader()->uFloat3("u_lightPos", light->pos());
  r->shader()->uFloat3("u_lightColor", light->color());
  // ----------------------------------------------- / Upload LIGHT data --- //

  // --- Upload CAMERA data ------------------------------------------------ //
  // ----------------------------------------------------------------------- //
  glm::mat4 matMV = camera->view() * r->model();
  r->shader()->uMat4("u_matMV", matMV);
  // ----------------------------------------------------------------------- //
  glm::mat4 matN = glm::transpose(glm::inverse(matMV));
  r->shader()->uMat4("u_matN", matN);
  // ----------------------------------------------------------------------- //
  r->shader()->uMat4("u_matM", r->model());
  r->shader()->uMat4("u_matV", camera->view());
  r->shader()->uMat4("u_matP", camera->proj());
  r->shader()->uMat4("u_matVP", camera->viewproj());
  r->shader()->uMat4("u_matMVP", camera->viewproj() * r->model());
  // ---------------------------------------------- / Upload CAMERA data --- //

  // --- DRAW CALL --------------------------------------------------------- //
  // ----------------------------------------------------------------------- //
  (grid) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
         : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // ----------------------------------------------------------------------- //
  r->draw();
  // ------------------------------------------------------- / DRAW CALL --- //
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
