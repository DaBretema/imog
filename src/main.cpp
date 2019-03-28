
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include "Logger.hpp"

#include "IO.hpp"
#include "Math.hpp"
// #include "Loader.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
using namespace brave;

// tmp

// #include <execution>
// #include <numeric>



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

// void DBG_BVH(const std::string& path) {
//   auto testbvh = loader::BVH(path);
//   int  i       = 0;
//   int  r       = 0;
//   for (auto frame : testbvh->frames) {
//     LOG("----------\nFrame{}", i++);
//     LOG("T : {}", glm::to_string(frame.translation));
//     for (auto rot : frame.rotations) LOG("R{} : {}", r++, glm::to_string(rot));
//     r = 0;
//   }
// };


// * ===================================================================== * //
// * ============================= / HELPERS ============================= * //
// * ===================================================================== * //


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //


// * ===================================================================== * //
// * ============================ APPLICATION ============================ * //
// * ===================================================================== * //


int main(int argc, char const* argv[]) {

  // std::vector<glm::vec3> A(1000, glm::vec3(1.f));
  // auto                   reduced = std::reduce(std::begin(A), std::end(A));

  // LOGD("vec sum => {}" glm::to_string(reduced));

  // return 0;
  // ---------------------------------------------------------
  // --- Initialization --------------------------------------

  Settings::init(Paths::settings);

  auto camera = std::make_shared<Camera>(Settings::mainCameraSpeed);

  IO::windowInit(camera);

  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);


  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------

  auto skeleton = Skeleton(camera, 0.33f);
  skeleton.addMotion("Idle", Motions::idle, loopMode::mirror);
  skeleton.addMotion("Run", Motions::run, loopMode::cycle);
  skeleton.animation();

  skeleton.onKey(GLFW_KEY_W,
                 [&]() {
                   skeleton.currMotion("Idle");
                   skeleton.moveF(false);
                 },
                 [&]() {
                   skeleton.currMotion("Run");
                   skeleton.moveF(true);
                 });


  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto updateFn = [&]() {
    camera->speed(Settings::mainCameraSpeed);
    light->pos(Settings::mainLightPos);
    light->color(Settings::mainLightColor);
    light->intensity(Settings::mainLightIntensity);
  };

  auto renderFn = [&]() {
    Shader::poolUpdate(camera, light);
    Renderable::poolDraw(camera);
    skeleton.draw();
    camera->frame();
  };

  IO::windowLoop(renderFn, updateFn);

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

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

Todo 1 : Allow multiple entries on same key state... (IO.cpp) unordered_multimap

Todo 2 : Make a motion pool access wrapper inside skeleton.

*/
// * ===================================================================== * //
// * ============================== / NOTES ============================== * //
// * ===================================================================== * //
