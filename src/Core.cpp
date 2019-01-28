
#include "../incl/Core.hpp"
#include "../incl/Settings.hpp"


namespace BRAVE {

bool Core::pause;
bool Core::threadsLive;

std::shared_ptr<Light>  Core::light;
std::shared_ptr<Camera> Core::camera;

void Core::init() {
  static std::once_flag OF_Init;
  try {
    std::call_once(OF_Init, [&]() {
      threadsLive = true;
      pause       = false;

      camera = std::move(std::make_shared<Camera>(Settings::mainCameraPos,
                                                  Settings::mainCameraSpeed));

      light = std::move(std::make_shared<Light>(Settings::mainLightPos,
                                                Settings::mainLightColor));

      IO::windowInit();
    });
  } catch (std::exception&) { DacLog_WARN("BRAVE was already initialized"); }
}

void Core::close() {
  pause       = true;
  threadsLive = false;
}

void Core::draw(const std::shared_ptr<Renderable>& r, bool grid) {

  if (camera == nullptr || light == nullptr) {
    DacLog_ERR("Camera or Light ar not defined");
    return;
  }

  DacLog_INFO("UPLOADING: Light");
  // --- Upload LIGHT data ---   @improve: use UBOs

  r->shader()->uFloat3("u_lightPos", light->pos());
  r->shader()->uFloat3("u_lightColor", light->color());

  DacLog_INFO("UPLOADING: Camera");
  // --- Upload CAMERA data ---

  glm::mat4 matMV = camera->view() * r->model();
  r->shader()->uMat4("u_matMV", matMV);
  glm::mat4 matN = glm::transpose(glm::inverse(matMV));
  r->shader()->uMat4("u_matN", matN);

  r->shader()->uMat4("u_matM", r->model());
  r->shader()->uMat4("u_matV", camera->view());
  r->shader()->uMat4("u_matP", camera->proj());

  r->shader()->uMat4("u_matVP", camera->viewproj());
  r->shader()->uMat4("u_matMVP", camera->viewproj() * r->model());

  // --- DRAW CALL ---

  DacLog_INFO("SETTING UP: Grid or not");
  (grid) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
         : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  DacLog_INFO("DRAW REQ to renderable");
  r->draw();
}

void Core::onUpdate(const _IO_FUNC& fn) { IO::windowLoop(fn); }

} // namespace BRAVE
