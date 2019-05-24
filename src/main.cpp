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

  //!! OK ALL it's working as desired
  //!! Just inject skeleton code here in skeleton class for be fluid :D

  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------

  auto sk = Skeleton(camera, 1.f, 1.f);

  // Load motions
  auto walk = Motion::create("walk", Motions::walk, loopMode::shortLoop, 3u);
  auto run  = Motion::create("run", Motions::run, loopMode::shortLoop, 3u);
  auto jump = Motion::create("jump", Motions::jump, loopMode::loop, 10u);
  auto idle = Motion::create("idle", Motions::dance, loopMode::loop, 10u);

  // To avoid diferent skeletons per motion
  run->joints  = walk->joints;
  jump->joints = walk->joints;
  idle->joints = walk->joints;

  // Motion addition
  sk.addMotion(jump);
  walk->linked = run;
  sk.addMotion(walk);
  sk.addMotion(idle);


  // Input setup
  std::once_flag _setKeysFlag;

  unsigned int moves    = 0u;
  bool         moving   = false;
  float        dirAngle = 0.f;

  bool jumpIn, jumpOut, moveIn, moveOut, moveRep, mustToggleCamera;

  auto step = [&]() { return sk.transform.front() * sk.step() * Math::vecXZ; };

  auto angle = [&](float dir) {
    auto sF = sk.transform.front() * Math::vecXZ;
    auto cF = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    cF      = glm::rotateY(cF * Math::vecXZ, glm::radians(dir));
    return glm::orientedAngle(sF, cF, Math::unitVecY) * Math::unitVecY;
  };

  //

  auto skT = std::shared_ptr<Transform>(&sk.transform);

  sk.userFn = [&]() {
    if (jumpIn) {
      jumpIn = false;
      step();
      sk.setMotion("jump");
      jumpOut = true;
    }

    if (jumpOut) {
      jumpOut = false;
      (moving) ? sk.setMotion("walk") : sk.setMotion("idle");
    }

    if (moveRep || moveIn) {
      sk.transform.rot += angle(dirAngle) * 10.f;
      sk.transform.pos += step();
    }

    if (moveIn) {
      moveIn = false;
      if ((moves++) > 0) return;
      sk.setMotion("walk");
      moving  = true;
      moveRep = true;
    }

    if (moveOut) {
      moveOut = false;
      if ((--moves) > 0) return;
      sk.setMotion("idle");
      moving = false;
    }

    if (mustToggleCamera) {
      mustToggleCamera  = false;
      sk.camera->target = (!sk.camera->target) ? skT : nullptr;
    }
  };

  auto moveOutFn = [&]() { moveOut = true; };
  sk.onKey(GLFW_KEY_F, [&]() { mustToggleCamera = true; });

  sk.onKey(GLFW_KEY_SPACE, [&]() { jumpIn = true; });
  sk.onKey(GLFW_KEY_W,
           [&]() {
             moveIn   = true;
             dirAngle = 0.f;
           },
           moveOutFn);
  sk.onKey(GLFW_KEY_D,
           [&]() {
             moveIn   = true;
             dirAngle = -90.f;
           },
           moveOutFn);
  sk.onKey(GLFW_KEY_S,
           [&]() {
             moveIn   = true;
             dirAngle = 180.f;
           },
           moveOutFn);
  sk.onKey(GLFW_KEY_A,
           [&]() {
             moveIn   = true;
             dirAngle = 90.f;
           },
           moveOutFn);
  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });
  sk.onKey(GLFW_KEY_6,
           [&]() { sk.speed = glm::clamp(sk.speed - 0.1f, 1.f, 10.f); });
  sk.onKey(GLFW_KEY_7,
           [&]() { sk.speed = glm::clamp(sk.speed + 0.1f, 1.f, 10.f); });
  sk.onKey(GLFW_KEY_8, [&]() {
    sk.decLinkedAlpha();
    sk.speed = glm::clamp(sk.speed - 0.1f, 1.f, 10.f);
  });
  sk.onKey(GLFW_KEY_9, [&]() {
    sk.incLinkedAlpha();
    sk.speed = glm::clamp(sk.speed + 0.1f, 1.f, 10.f);
  });
  sk.onKey(GLFW_KEY_1,
           [&]() { walk->linked = (!walk->linked) ? run : nullptr; });

  sk.animate();

  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto initFn = [&]() {
    Renderable::getByName("Floor")->transform.pos.y =
        sk.footHeight() - sk.transform.pos.y;
  };

  auto updateFn = [&]() { camera->speed(Settings::mainCameraSpeed); };

  auto renderFn = [&]() {
    sk.draw();
    camera->frame();
    Shader::poolUpdate(camera);
    Renderable::poolDraw(camera);

    static std::once_flag initFlag;
    std::call_once(initFlag, initFn);

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
