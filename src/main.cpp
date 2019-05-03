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
  // box->transform.scl *= 2.f;
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

  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);

  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------
  auto sk = Skeleton(camera, 0.5f);
  {
    // auto idle = Motion::create("idle", Motions::idle, loopMode::loop, 25u);
    // sk.onKey(GLFW_KEY_1, [&]() { sk.setMotion("idle"); });
    // sk.addMotion(idle);

    auto walk = Motion::create("walk", Motions::walk, loopMode::shortLoop);
    // sk.onKey(GLFW_KEY_2, [&]() { sk.setMotion("walk"); });
    sk.addMotion(walk);
    //
    // auto run  = Motion::create("run", Motions::run, loopMode::shortLoop);
    // sk.onKey(GLFW_KEY_3, [&]() { sk.setMotion("run"); });
    // sk.addMotion(run);
    //
    // auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 25u);
    // sk.onKey(GLFW_KEY_4, [&]() { sk.setMotion("jump"); });
    // sk.addMotion(jump);

    // sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });
  }

  glm::vec3 cameraFront;

  {
    auto angle = [&](const glm::vec3& v2) {
      auto skF  = sk.transform.front();
      auto dir1 = glm::compAdd(skF) < glm::compAdd(v2);
      auto dir2 = sk.transform.rot.y < 0;
      auto dir  = (dir1) ? 1.f : -1.f;
      LOGD("\n{}\n{}\n{}",
           glm::to_string(sk.transform.rot),
           glm::to_string(camera->pivot.rot),
           glm::to_string(camera->transform.rot));
      auto angle = dir * glm::degrees(glm::angle(skF.xz(), v2.xz()));
      return angle * Math::unitVecY;
    };
    auto angleF = [&]() { return angle(cameraFront); };
    auto angleR = [&]() { return angleF() + Math::unitVecY * 90.f; };
    auto angleB = [&]() { return angleF() + Math::unitVecY * -180.f; };
    auto angleL = [&]() { return angleF() + Math::unitVecY * 270.f; };

    auto step = [&]() { return sk.transform.front() * sk.step(); };

    auto goF = [&]() {
      sk.transform.pos += step();
      sk.transform.rot += angleF();
    };
    auto goB = [&]() {
      sk.transform.pos -= step();
      sk.transform.rot += angleB();
    };
    sk.onKey(GLFW_KEY_W, goF, emptyFn, goF);
    sk.onKey(GLFW_KEY_S, goB, emptyFn, goB);

    // auto stepR = [&]() { return sk.transform.right() * sk.step(); };
    auto goR = [&]() {
      // sk.transform.pos += step();
      sk.transform.rot += angleR();
    };
    auto goL = [&]() {
      // sk.transform.pos += step();
      sk.transform.rot += angleL();
    };
    sk.onKey(GLFW_KEY_D, goR, emptyFn, goR);
    sk.onKey(GLFW_KEY_A, goL, emptyFn, goL);
  }
  sk.animate();


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
    camera->frame();

    sk.draw();
    Renderable::poolDraw(camera);
    cameraFront = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    Shader::poolUpdate(camera, light);

    DBG_VEC(camera, cameraFront, Colors::cyan, sk.transform.pos);
    DBG_VEC(camera, camera->pivot.right(), Colors::yellow, sk.transform.pos);
    DBG_VEC(camera, sk.transform.front(), Colors::blue, sk.transform.pos);
    DBG_VEC(camera, sk.transform.right(), Colors::red, sk.transform.pos);

    DBG_POINT(
        camera, sk.transform.front() * 10.f, Colors::green, sk.transform.pos);
    DBG_POINT(
        camera, sk.transform.right() * 10.f, Colors::green, sk.transform.pos);
    DBG_POINT(
        camera, sk.transform.front() * -10.f, Colors::white, sk.transform.pos);
    DBG_POINT(
        camera, sk.transform.right() * -10.f, Colors::white, sk.transform.pos);

    DBG_POINT(camera, cameraFront * 10.f, Colors::green, sk.transform.pos);
    DBG_POINT(
        camera, camera->pivot.right() * 10.f, Colors::green, sk.transform.pos);
    DBG_POINT(camera, cameraFront * -10.f, Colors::white, sk.transform.pos);
    DBG_POINT(
        camera, camera->pivot.right() * -10.f, Colors::white, sk.transform.pos);
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

if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
    glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
{
    call_some_function();
}
*/
