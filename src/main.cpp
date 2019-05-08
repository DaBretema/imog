#include "IO.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
using namespace brave;

// * ===================================================================== * //
void DBG_VEC(std::shared_ptr<brave::Camera> camera,
             glm::vec3                      P,
             glm::vec3                      color  = glm::vec3(1.f, 0.f, 0.f),
             glm::vec3                      center = glm::vec3(0.f)) {
  auto P1 = center + P * 0.5f;
  auto P2 = center + P * -0.5f;

  auto cyl = Renderable::cylBetween2p(P1, P2, 10.f);
  cyl->color(color);
  cyl->draw(camera);
};
// * ===================================================================== * //
void DBG_POINT(std::shared_ptr<brave::Camera> camera,
               glm::vec3                      P,
               glm::vec3                      color  = glm::vec3(1.f, 0.f, 0.f),
               glm::vec3                      center = glm::vec3(0.f)) {
  auto box           = Renderable::getByName("Cube");
  box->transform.pos = center + P;
  box->color(color);
  box->draw(camera);
};
// * ===================================================================== * //

int main(int argc, char const* argv[]) {

  // ---------------------------------------------------------
  // --- Initialization --------------------------------------

  Settings::init(Paths::settings);
  auto camera = std::make_shared<Camera>(Settings::mainCameraSpeed,
                                         Settings::mainCameraFov);
  IO::windowInit(camera);

  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------
  auto sk = Skeleton(camera, 0.3f, 1.f);

  // auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 25u);
  // sk.onKey(GLFW_KEY_SPACE,
  //          [&]() { sk.setMotion("jump"); },
  //          [&]() { sk.setMotion("walk"); });
  // sk.addMotion(jump);

  // auto walk      = Motion::create("walk", Motions::walk, loopMode::shortLoop);
  // bool walking   = false;
  // auto startWalk = [&]() {
  //   sk.setMotion("walk");
  //   walking = true;
  // };
  // auto stopWalk = [&]() {
  //   sk.setMotion("idle");
  //   walking = false;
  // };
  // sk.addMotion(walk);

  // auto run       = Motion::create("run", Motions::run, loopMode::shortLoop);
  // bool toggleRun = false;
  // sk.onKey(GLFW_KEY_R, [&]() {
  //   toggleRun = !toggleRun;
  //   (toggleRun) ? sk.setMotion("run")
  //               : (walking) ? sk.setMotion("walk") : sk.setMotion("idle");
  // });
  // sk.addMotion(run);

  auto idle = Motion::create("idle", Motions::idle, loopMode::loop, 25u);
  sk.addMotion(idle);



  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });

  {
    // auto step = [&]() { return sk.transform.front() * sk.step(); };

    auto angle = [&](float rotAngle) {
      sk.transform.pos +=
          sk.transform.front() * sk.step() * 2.f; // Just 'd.r.y.'

      auto sF = sk.transform.front();
      auto cF = glm::rotateY(camera->pivot.front(), glm::radians(rotAngle));

      auto angle = glm::angle(sF, cF) * 2.f;
      auto cross = glm::cross(sF, cF);
      auto dot   = glm::dot(cross, Math::unitVecY);

      return Math::unitVecY * ((dot < 0) ? -angle : angle);
    };

    auto goF = [&]() { sk.transform.rot += angle(0.f); };
    auto goR = [&]() { sk.transform.rot += angle(-90.f); };
    auto goB = [&]() { sk.transform.rot += angle(180.f); };
    auto goL = [&]() { sk.transform.rot += angle(90.f); };



    // sk.onKey(GLFW_KEY_W,
    //          [&]() {
    //            goF();
    //            startWalk();
    //          },
    //          stopWalk,
    //          goF);
    // sk.onKey(GLFW_KEY_D,
    //          [&]() {
    //            goR();
    //            startWalk();
    //          },
    //          stopWalk,
    //          goR);
    // sk.onKey(GLFW_KEY_S,
    //          [&]() {
    //            goB();
    //            startWalk();
    //          },
    //          stopWalk,
    //          goB);
    // sk.onKey(GLFW_KEY_A,
    //          [&]() {
    //            goL();
    //            startWalk();
    //          },
    //          stopWalk,
    //          goL);
  }
  sk.animate();

  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto updateFn = [&]() { camera->speed(Settings::mainCameraSpeed); };

  auto renderFn = [&]() {
    camera->frame();

    sk.draw();

    // DBG_POINT(camera, glm::vec3(0.f), Colors::teal);
    // DBG_POINT(camera, glm::vec3(0.2f, 0.f, 0.2f), Colors::red);

    Renderable::poolDraw(camera);
    Shader::poolUpdate(camera);
  };

  IO::windowLoop(renderFn, updateFn);

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

  // Skeleton invoke a thread that use render, shader and texture pools.
  // So if we don't remove skeleton object before remove pools' content and
  // let that OS' memory manager remove unfree data. May ocur

  delete &sk;
  return 0;
}

/*

<<SELF>> SIMILARITY MATRIX`
ROTACIONES RELATIVAS

if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
    glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
{
    call_some_function();
}
*/
