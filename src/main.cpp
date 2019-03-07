
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include "IO.hpp"
#include "Math.hpp"
#include "Loader.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
using namespace brave;



// * ===================================================================== * //
// * ============================== HELPERS ============================== * //
// * ===================================================================== * //


// ========================================================================= //
// ========================================================================= //
// Draw a vector in space
// ========================================================================= //

void DBG_VEC(std::shared_ptr<brave::Camera> camera,
             glm::vec3                      vec,
             glm::vec3                      color = glm::vec3(1, 0, 0)) {

  auto DBG = Renderable::create(false, "DBG", Figures::cube, "", Colors::red);
  DBG->transform.scl = glm::vec3(0.25f);
  DBG->color(color);
  for (auto i = -25; i < 25; i++) {
    DBG->transform.pos = vec * (0.1f * i);
    DBG->draw(camera);
  }
};

// ========================================================================= //
// ========================================================================= //
// Dump bvh content
// ========================================================================= //

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


// * ===================================================================== * //
// * ============================= / HELPERS ============================= * //
// * ===================================================================== * //


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //


// * ===================================================================== * //
// * ============================ APPLICATION ============================ * //
// * ===================================================================== * //


int main(int argc, char const* argv[]) {

  Settings::init(Paths::settings);
  auto camera = std::make_shared<Camera>(Settings::mainCameraSpeed);
  IO::windowInit(camera);

  // TODO: If not defined should use camera as light
  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);

  Renderable::create(true, "Floor", Figures::plane, Textures::chess);
  Renderable::getByName("Floor")->transform.pos -= glm::vec3(0, 6.0f, 0);
  Renderable::getByName("Floor")->transform.scl = glm::vec3(500.f, 1.f, 500.f);


  // TODO: Free camera move if is not defined
  auto skeleton = Skeleton(camera, 0.33f);
  skeleton.addMotion("Idle", Motions::idle);
  skeleton.addMotion("Run", Motions::run);
  skeleton.animation();

  IO_DefineKeyStates(GLFW_KEY_W,
                     skeleton.currMotion("Idle"),
                     skeleton.currMotion("Run"),
                     skeleton.moveFront());

  auto updateFn = [&]() {
    camera->speed(Settings::mainCameraSpeed);
    light->pos(Settings::mainLightPos);
    light->color(Settings::mainLightColor);
    light->intensity(Settings::mainLightIntensity);
  };


  auto renderFn = [&]() {
    // Compute scene frame
    camera->frame();
    // Update global data to shaders
    for (const auto& s : Shader::pool) { s->update(camera, light); }
    // Update specific renderable data to renderable shader and draw
    for (const auto& r : Renderable::pool) {
      if (r->globalDraw) r->draw(camera);
    }
    // Draw, desired !globalDraw renderables
    skeleton.draw();
  };

  IO::windowLoop(renderFn, updateFn);
  return 0;
}

// * ===================================================================== * //
// * =========================== / APPLICATION =========================== * //
// * ===================================================================== * //


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //


// * ===================================================================== * //
// * =============================== NOTES =============================== * //
// * ===================================================================== * //
/*

 Probablemente rompa de manera aleatoria al mover, pq no se resetean los
 contadores de la jerarquía al cambiar de Motion :D

*/
// * ===================================================================== * //
// * ============================== / NOTES ============================== * //
// * ===================================================================== * //
