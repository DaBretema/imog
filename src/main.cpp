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

  // auto sk1 = Skeleton(0.33f);
  // sk1.setAnimFromBVH("run", Motions::run);
  // sk1.animation();
  auto re              = Renderable::getByName("cube_pivot");
  Core::camera->target = std::shared_ptr<Transform>(&re->transform);

  Core::onUpdate([&]() {
    Core::frame();

    // Core::camera->pos(re->pos() - re->model()[2].xyz() * -3.f);
    // dInfo("pos_camera: {}", glm::to_string(Core::camera->pos()));


    // sk1.draw();
    // DBG_VEC(Core::camera->frontY(), Colors::green);
    // DBG_VEC(sk1.front());
    // Core::light->pos(sk1->rootPos() + glm::vec3(0.f, 12.5f, 0.f));
    // cube->draw();
  });



  return 0;
}
