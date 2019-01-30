#include "Core.hpp"
#include "Prefabs.hpp"
#include "Settings.hpp"
#include "helpers/Paths.hpp"


int main(int argc, char const* argv[]) {

  BRAVE::Settings::init(BRAVE::Paths::Settings);
  BRAVE::Core::init();

  auto floor = BRAVE::Prefab::plane(BRAVE::Colors::Cyan,
                                    glm::vec3{0.f, -18.f, 0.f},
                                    glm::vec3{0},
                                    glm::vec3{20.f, 20.f, 10.f});

  BRAVE::Core::onUpdate([&]() {
    auto cCam   = BRAVE::Core::camera;
    auto cLight = BRAVE::Core::light;

    // Compute camera data
    cCam->frame();

    // Upload render independent data
    for (const auto& s : BRAVE::Shader::pool) {
      s->uFloat3("u_lightPos", cLight->pos());
      s->uFloat3("u_lightColor", cLight->color());

      s->uMat4("u_matV", cCam->view());
      s->uMat4("u_matP", cCam->proj());
      s->uMat4("u_matVP", cCam->viewproj());
    }

    // Upload render dependent data
    for (const auto& r : BRAVE::Renderable::pool) {
      glm::mat4 matMV = cCam->view() * r->model();
      glm::mat4 matN  = glm::transpose(glm::inverse(matMV));

      auto s = r->shader();
      s->uMat4("u_matMVP", cCam->viewproj() * r->model());
      s->uMat4("u_matM", r->model());
      s->uMat4("u_matMV", matMV);
      s->uMat4("u_matN", matN);
      r->draw();
    }

    // BRAVE::Core::draw(floor);
  });

  return 0;
}
