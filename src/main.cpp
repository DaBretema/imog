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
  DBG->transform.scl = glm::vec3(0.25f);
  DBG->color(color);
  for (auto i = -25; i < 25; i++) {
    DBG->transform.pos = vec * (0.1f * i);
    DBG->draw(Core::camera);
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

  // Avoid this, computing the Skeleton height and moving up the skeleton
  // a half of its height.
  Renderable::getByName("Floor")->transform.pos -= glm::vec3(0, 6.0f, 0);
  Renderable::getByName("Floor")->transform.scl = glm::vec3(10.f, 1.f, 10.f);

  dInfo("1");
  auto sk1 = Skeleton(Core::camera, 0.33f);
  sk1.setAnimFromBVH("Idle", Motions::idle);
  sk1.setAnimFromBVH("Run", Motions::run);
  dInfo("2");
  sk1.animation();
  dInfo("3");

  Core::onUpdate([&]() {
    Core::frame();
    sk1.draw();
  });



  return 0;
}
