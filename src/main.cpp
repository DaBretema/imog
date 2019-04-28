#include "IO.hpp"
#include "Math.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
using namespace brave;


// * ===================================================================== * //
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
// * ===================================================================== * //


int main(int argc, char const* argv[]) {

  // ---------------------------------------------------------
  // --- Initialization --------------------------------------

  Settings::init(Paths::settings);

  auto camera = std::make_shared<Camera>(Settings::mainCameraSpeed,
                                         Settings::mainCameraFov);

  IO::windowInit(camera);

  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);

  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------

  // auto idle = Motion::create("idle", Motions::idle, loopMode::shortLoop, 10u);
  auto walk = Motion::create("walk", Motions::walk, loopMode::shortLoop, 10u);
  auto run  = Motion::create("run", Motions::run, loopMode::shortLoop, 10u);
  // auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 10u);

  auto sk = Skeleton(camera, 0.5f);
  // sk.addMotion(idle);
  sk.addMotion(walk);
  sk.addMotion(run);
  // sk.addMotion(jump);
  sk.animate();

  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });
  IO::keyboardAddAction(
      GLFW_KEY_0, IO::kbState::repeat, [&]() { sk.play = !sk.play; });

  sk.onKey(GLFW_KEY_1, [&]() { sk.setMotion("idle"); });
  sk.onKey(GLFW_KEY_2, [&]() { sk.setMotion("walk"); });
  sk.onKey(GLFW_KEY_3, [&]() { sk.setMotion("run"); });
  sk.onKey(GLFW_KEY_4, [&]() { sk.setMotion("jump"); });

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
    sk.draw();
    camera->frame();
    Shader::poolUpdate(camera, light);
    Renderable::poolDraw(camera);
  };

  IO::windowLoop(renderFn, updateFn);

  // Skeleton invoke a thread that use render, shader and texture pools.
  // So if we don't remove skeleton object before remove pools' content and
  // let that OS' memory manager remove unfree data. May ocur
  delete &sk;

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

  return 0;
}



/*

<<SELF>> SIMILARITY MATRIX`
ROTACIONES RELATIVAS


*/
