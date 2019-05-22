#include "IO.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
#include "helpers/Debug.hpp"
using namespace brave;

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

  auto sk = Skeleton(camera, 0.6f, 1.f);

  // Load motions
  auto walk = Motion::create("walk", Motions::walk, loopMode::shortLoop, 5u);
  auto run  = Motion::create("run", Motions::run, loopMode::shortLoop, 5u);
  auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 10u);
  auto idle = Motion::create("idle", Motions::tPose, loopMode::loop, 25u);

  // To avoid diferent skeletons per motion
  run->joints  = walk->joints;
  jump->joints = walk->joints;
  idle->joints = walk->joints;

  // Motion addition
  sk.addMotion(jump);
  // walk->linked = run;
  sk.addMotion(walk);
  sk.addMotion(idle);

  // Input setup
  bool         moving = false;
  unsigned int moves  = 0u;

  auto step = [&]() { return sk.transform.front() * sk.step() * Math::vecXZ; };

  auto angle = [&](float dir) {
    auto sF = sk.transform.front() * Math::vecXZ;
    auto cF = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    cF      = glm::rotateY(cF * Math::vecXZ, glm::radians(dir));
    return glm::orientedAngle(sF, cF, Math::unitVecY) * Math::unitVecY;
  };

  auto jumpIn = [&]() {
    sk.setMotion("jump");
    sk.allowedTrans = Math::unitVecY; // Allow jump
  };

  auto jumpOut = [&]() {
    (moving) ? sk.setMotion("walk") : sk.setMotion("idle");
    sk.allowedTrans = Math::nullVec; // Disallow jump
  };

  auto moveRep = [&](float dir) {
    sk.transform.pos += step();
    sk.transform.rot += angle(dir);
  };

  auto moveIn = [&](float dir) {
    moveRep(dir);
    if ((moves++) > 0) return;
    sk.setMotion("walk");
    moving = true;
  };

  auto moveOut = [&]() {
    if ((--moves) > 0) return;
    sk.setMotion("idle");
    moving = false;
  };

  auto F = [&](const auto& fn) { return [&]() { fn(0.f); }; };
  auto R = [&](const auto& fn) { return [&]() { fn(-90.f); }; };
  auto B = [&](const auto& fn) { return [&]() { fn(180.f); }; };
  auto L = [&](const auto& fn) { return [&]() { fn(90.f); }; };

  sk.onKey(GLFW_KEY_SPACE, jumpIn, jumpOut);
  sk.onKey(GLFW_KEY_W, F(moveIn), moveOut, F(moveRep));
  sk.onKey(GLFW_KEY_D, R(moveIn), moveOut, R(moveRep));
  sk.onKey(GLFW_KEY_S, B(moveIn), moveOut, B(moveRep));
  sk.onKey(GLFW_KEY_A, L(moveIn), moveOut, L(moveRep));
  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });
  sk.onKey(GLFW_KEY_8, [&]() { sk.decLinkedAlpha(); });
  sk.onKey(GLFW_KEY_9, [&]() { sk.incLinkedAlpha(); });
  sk.onKey(GLFW_KEY_1,
           [&]() { walk->linked = (!walk->linked) ? run : nullptr; });

  sk.animate();

  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto toggleCamera = [&]() {
    if (!sk.camera->target)
      sk.camera->target = std::shared_ptr<Transform>(&sk.transform);
    else if (sk.camera->target)
      sk.camera->target = nullptr;
  };
  sk.onKey(GLFW_KEY_F, toggleCamera);

  auto updateFn = [&]() { camera->speed(Settings::mainCameraSpeed); };

  auto renderFn = [&]() {
    sk.draw();
    camera->frame();
    Shader::poolUpdate(camera);
    Renderable::poolDraw(camera);

    // if (glfwGetKey())
  };

  // std::thread([]() { system("python plotter.py"); }).detach();

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
