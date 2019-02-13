#include "Core.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"
#include "Skeleton.hpp"

#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

// temp
#include "Math.hpp"


using namespace brave;

int main(int argc, char const* argv[]) {
  Core::init(Paths::settings);

  auto sk1 = Skeleton::create(Motions::run, 0.33f);

  IO::keyboardAddAction(GLFW_KEY_1, [&]() { sk1->play(); });
  IO::keyboardAddAction(GLFW_KEY_2, [&]() { sk1->stop(); });



  // auto dbg = Renderable::create(false,
  //                               "dbg",
  //                               Figures::cube,
  //                               "",
  //                               Colors::red,
  //                               Shader::getByName(Shaders::light));

  // auto debugVec = [&dbg](glm::vec3 vec, glm::vec3 color = glm::vec3(1, 0, 0)) {
  //   dbg->scl(glm::vec3(0.25f));
  //   dbg->color(color);
  //   for (auto i = -25; i < 25; i++) {
  //     dbg->pos(vec * (0.1f * i));
  //     dbg->draw();
  //   }
  // };

  // auto debugPoint = [&dbg](glm::vec3 point,
  //                          glm::vec3 color = glm::vec3(1, 0, 0)) {
  //   dbg->scl(glm::vec3(1.f));
  //   dbg->color(color);
  //   dbg->pos(point);
  //   dbg->draw();
  // };


  Core::onUpdate([&]() {
    Core::frame();
    sk1->draw();
    Core::light->pos(sk1->rootPos() + glm::vec3(0.f, 12.5f, 0.f));
  });

  return 0;
}
