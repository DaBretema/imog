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
#include "Loader.hpp"


using namespace brave;


void DBG_VEC(glm::vec3 vec, glm::vec3 color = glm::vec3(1, 0, 0)) {
  auto DBG = Renderable::create(false, "DBG", Figures::cube, "", Colors::red);
  DBG->scl(glm::vec3(0.25f));
  DBG->color(color);
  for (auto i = -25; i < 25; i++) {
    DBG->pos(vec * (0.1f * i));
    DBG->draw();
  }
};

void DBG_BVH(const std::string& path) {
  auto testbvh = loader::BVH(path);
  int  i       = 0;
  int  r       = 0;
  for (auto frame : testbvh->frames) {
    dPrint("----------\nFrame{}", i++);
    dPrint("T : {}", glm::to_string(frame.translation));
    for (auto rot : frame.rotations)
      dPrint("R{} : {}", r++, glm::to_string(rot));
    r = 0;
  }
};



// ------------- //
//  ENTRY-POINT  //
// ------------- //


int main(int argc, char const* argv[]) {
  Core::init(Paths::settings);


  // auto sk1 = Skeleton::create(Motions::run, 0.33f);

  // IO::keyboardAddAction(GLFW_KEY_1, [&]() { sk1->play(); });
  // IO::keyboardAddAction(GLFW_KEY_2, [&]() { sk1->stop(); });


  // auto cube =
  //     Renderable::create(false, "cube", Figures::monkey, "", Colors::white);
  // cube->scl(cube->scl() * 3.f);
  // cube->pos(0, 2.f, 0);

  // // Pro
  // float step     = 1.5f;
  // auto  getFront = [&]() { return glm::normalize(cube->model()[2]); };

  // // Keyboard, add flag for RELEASE or PRESS
  // IO::keyboardAddAction(GLFW_KEY_I,
  //                       [&]() { cube->accumPos(getFront() * step); });
  // IO::keyboardAddAction(GLFW_KEY_K,
  //                       [&]() { cube->accumPos(-getFront() * step); });
  // IO::keyboardAddAction(GLFW_KEY_J, [&]() { cube->accumRot(0, -1.f, 0); });
  // IO::keyboardAddAction(GLFW_KEY_L, [&]() { cube->accumRot(0, 1.f, 0); });



  // auto debugPoint = [&dbg](glm::vec3 point,
  //                          glm::vec3 color = glm::vec3(1, 0, 0)) {
  //   dbg->scl(glm::vec3(1.f));
  //   dbg->color(color);
  //   dbg->pos(point);
  //   dbg->draw();
  // };

  auto sk1 = Skeleton(0.33f);
  sk1.setAnimFromBVH("run", Motions::run);
  sk1.animation();

  Core::onUpdate([&]() {
    Core::frame();
    sk1.draw();
    // Core::light->pos(sk1->rootPos() + glm::vec3(0.f, 12.5f, 0.f));
    // cube->draw();
  });



  return 0;
}
