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

  float dirAngle    = 0.f;
  float rotSpeed    = 5.f;
  bool  changeSpeed = true;
  auto  skt         = std::shared_ptr<Transform>(&sk.transform);
  bool  _jump, mustToggleCamera, front, right, back, left;

  auto movesCount = [&]() { return front + right + back + left; };
  auto isMoving   = [&]() { return front || right || back || left; };

  auto step = [&]() { return sk.transform.front() * sk.step() * Math::vecXZ; };

  auto angle = [&](float dir) {
    auto sF = sk.transform.front() * Math::vecXZ;
    auto cF = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    cF      = glm::rotateY(cF * Math::vecXZ, glm::radians(dir));
    return glm::orientedAngle(sF, cF, Math::unitVecY) * Math::unitVecY;
  };

  //
  sk.userFn = [&]() {
    // if (_jump) {
    //   sk.setMotion("jump");
    //   if (changeSpeed) {
    //     changeSpeed = false;
    //     sk.speed *= 0.5f;
    //   }
    // } else {
    //   (isMoving()) ? sk.setMotion("walk") : sk.setMotion("idle");
    //   if (!changeSpeed) {
    //     changeSpeed = true;
    //     sk.speed *= 2.f;
    //   }
    // }
    (_jump) ? [&](){sk.setMotion("jump"); if (changeSpeed) {sk.speed *= 0.5f; changeSpeed = false;} }()
              : [&](){(isMoving()) ? sk.setMotion("walk") : sk.setMotion("idle"); if (!changeSpeed) {sk.speed *= 2.0f; changeSpeed = true;}}();
    // (_jump) ? sk.setMotion("jump")
    //         : (isMoving()) ? sk.setMotion("walk") : sk.setMotion("idle");

    if (front) sk.transform.rot += angle(0.f) * rotSpeed;
    if (right) sk.transform.rot += angle(270.f) * rotSpeed;
    if (back) sk.transform.rot += angle(180.f) * rotSpeed;
    if (left) sk.transform.rot += angle(90.f) * rotSpeed;
    if (isMoving() || _jump) sk.transform.pos += step();

    if (mustToggleCamera) {
      mustToggleCamera  = false;
      sk.camera->target = (!sk.camera->target) ? skt : nullptr;
    }
  };

  sk.onKey(GLFW_KEY_F, [&]() { mustToggleCamera = true; });
  sk.onKey(GLFW_KEY_SPACE, [&]() { _jump = true; }, [&]() { _jump = false; });
  sk.onKey(GLFW_KEY_W, [&]() { front = true; }, [&]() { front = false; });
  sk.onKey(GLFW_KEY_D, [&]() { right = true; }, [&]() { right = false; });
  sk.onKey(GLFW_KEY_S, [&]() { back = true; }, [&]() { back = false; });
  sk.onKey(GLFW_KEY_A, [&]() { left = true; }, [&]() { left = false; });
  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });
  sk.onKey(GLFW_KEY_6,
           [&]() { sk.speed = glm::clamp(sk.speed - 0.1f, 1.f, 3.f); });
  sk.onKey(GLFW_KEY_7,
           [&]() { sk.speed = glm::clamp(sk.speed + 0.1f, 1.f, 3.f); });
  sk.onKey(GLFW_KEY_8, [&]() {
    sk.decLinkedAlpha();
    sk.speed = glm::clamp(sk.speed - 0.1f, 1.f, 2.f);
  });
  sk.onKey(GLFW_KEY_9, [&]() {
    sk.incLinkedAlpha();
    sk.speed = glm::clamp(sk.speed + 0.1f, 1.f, 2.f);
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
  };

  // This could be improved creating a Python C++ extension
  // Plot the data generated during motions interpolation
  if (Settings::showPlots) {
    std::string pyCmd = "python plotter.py " + Motion::plotFolder();
    std::thread([&pyCmd]() { system(pyCmd.c_str()); }).detach();
  }

  // Init winow loop
  IO::windowVisibility(true); //! Do not remove. Avoid white screen on loading.
  IO::windowLoop(renderFn, updateFn);

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

  // Skeleton invoke a thread that use render, shader and texture pools.
  // So if we don't remove skeleton object before remove pools' content and
  // let that OS' memory manager remove unfree data. May ocur

  // sk.~Skeleton();
  // delete &sk;
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
