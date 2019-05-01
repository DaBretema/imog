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
  auto auxP = center + P;
  auto P1   = center + P * (0.1f * 100.f);
  auto P2   = center + P * (0.1f * -100.f);
  auto cyl  = Renderable::getByName("Bone");
  {
    cyl->transform.pos = (P1 + P2) * 0.5f;
    // ---
    auto C1                 = cyl->transform.pos + glm::vec3(0, 0.5f, 0);
    auto C2                 = cyl->transform.pos - glm::vec3(0, 0.5f, 0);
    auto vP                 = glm::normalize(P1 - P2);
    auto vC                 = glm::normalize(C1 - C2);
    cyl->transform.rotAngle = glm::angle(vC, vP);
    cyl->transform.rotAxis  = glm::cross(vC, vP);
    // ---
    cyl->transform.scl = glm::vec3{1.f, 10.f, 1.0f};
  }
  cyl->color(color);
  cyl->draw(camera);
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
  {

    // float auxAngleF = 0.f;
    auto angleF = [&]() {
      return glm::angle(sk.transform.front(), camera->pivot.front());
    };
    auto stepF = [&]() { return sk.transform.front() * sk.step(); };
    // auto stepF = [&]() { return sk.transform.front() * sk.step(); };
    auto goF = [&]() {
      sk.transform.pos += stepF();
      // sk.transform.rot += Math::unitVecY * angleF();
    };
    auto goB = [&]() {
      sk.transform.pos -= stepF();
      // sk.transform.rot += Math::unitVecY * -angleF();
    };
    sk.onKey(GLFW_KEY_W, goF, emptyFn, goF);
    sk.onKey(GLFW_KEY_S, goB, emptyFn, goB);

    // float auxAngleR = 0.f;
    auto angleR = [&]() {
      return glm::angle(sk.transform.front(), camera->pivot.right());
    };
    // auto stepR = [&]() { return sk.transform.right() * sk.step(); };
    auto goR = [&]() {
      // sk.transform.pos += stepF();
      sk.transform.rot += Math::unitVecY * angleR();
    };
    auto goL = [&]() {
      // sk.transform.pos += stepF();
      sk.transform.rot += Math::unitVecY * -angleR();
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
    Shader::poolUpdate(camera, light);

    auto cameraFront = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    DBG_VEC(camera, cameraFront, Colors::cyan, sk.transform.pos);
    DBG_VEC(camera, camera->pivot.right(), Colors::pink, sk.transform.pos);
    DBG_VEC(camera, sk.transform.front(), Colors::blue, sk.transform.pos);
    DBG_VEC(camera, sk.transform.right(), Colors::red, sk.transform.pos);
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
