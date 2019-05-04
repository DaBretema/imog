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

  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);

  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------
  auto sk = Skeleton(camera, 0.5f, 1.35f);
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
    auto step = [&]() { return sk.transform.front() * sk.step(); };

    auto angle = [&]() {
      sk.transform.pos += step(); // Just here for 'd.r.y.'

      auto skF = sk.transform.front();
      auto cF  = glm::rotateY(camera->pivot.right(), glm::radians(90.f));

      auto angle = glm::angle(skF, cF);
      auto cross = glm::cross(skF, cF);
      auto dot   = glm::dot(cross, Math::unitVecY);
      return Math::unitVecY * ((dot < 0) ? -angle : angle);
    };
    auto goF = [&]() { sk.transform.rot += angle(); };
    auto goR = [&]() { sk.transform.rot += angle() + Math::unitVecY * -90.f; };
    auto goB = [&]() { sk.transform.rot += angle() + Math::unitVecY * 180.f; };
    auto goL = [&]() { sk.transform.rot += angle() + Math::unitVecY * 90.f; };

    sk.onKey(GLFW_KEY_W, goF, emptyFn, goF);
    sk.onKey(GLFW_KEY_D, goR, emptyFn, goR);
    sk.onKey(GLFW_KEY_S, goB, emptyFn, goB);
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
    Shader::poolUpdate(camera, light);

    cameraFront = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    DBG_VEC(camera, Math::unitVecZ, Colors::black, sk.transform.pos);
    DBG_VEC(camera, Math::unitVecX, Colors::white, sk.transform.pos);
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
