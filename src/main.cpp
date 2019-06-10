#include "gltools_IO.hpp"
#include "cpptools_Logger.hpp"
#include "gltools_Math.hpp"
#include "Settings.hpp"
#include "mgtools_Skeleton.hpp"
#include "gltools_Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
#include "helpers/Debug.hpp"
using namespace imog;

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

  auto sk = Skeleton(camera, 1.f, 1.f);

  // Motions for interactive state
  auto walk = Motion::create("walk", Motions::walk, loopMode::shortLoop, 10u);
  auto run  = Motion::create("run", Motions::run, loopMode::shortLoop, 0u);
  walk->linked = run;
  auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 10u);


  // Motions for static state
  std::string staticMoName = "tPose";
  auto tPose    = Motion::create("tPose", Motions::tPose, loopMode::none, 0u);
  auto dance    = Motion::create("dance", Motions::dance, loopMode::loop, 10u);
  auto backflip = Motion::create(
      "backflip", Motions::backflip, loopMode::loopAndLockX, 0u, false);

  sk.onKey(GLFW_KEY_1, [&]() { staticMoName = "tPose"; });
  sk.onKey(GLFW_KEY_2, [&]() { staticMoName = "dance"; });
  sk.onKey(GLFW_KEY_3, [&]() { staticMoName = "backflip"; });
  sk.onKey(GLFW_KEY_4, [&]() { staticMoName = "jump"; });
  sk.onKey(GLFW_KEY_5, [&]() { staticMoName = "walk"; });
  sk.onKey(GLFW_KEY_6, [&]() { staticMoName = "run"; });

  // Motion addition
  sk.addMotion(walk);
  sk.addMotion(run);
  sk.addMotion(jump);
  sk.addMotion(tPose);
  sk.addMotion(dance);
  sk.addMotion(backflip);

  // Input setup
  float rotSpeed    = 5.f;
  bool  changeSpeed = true;
  bool  _jump, mustToggleCamera, front, right, back, left;
  _jump = mustToggleCamera = front = right = back = left = false;

  auto isMoving = [&]() { return front || right || back || left; };

  auto step = [&]() { return sk.transform.front() * sk.step() * Math::vecXZ; };

  auto angle = [&](float dir) {
    auto sF = sk.transform.front() * Math::vecXZ;
    auto cF = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
    cF      = glm::rotateY(cF * Math::vecXZ, glm::radians(dir));
    return glm::orientedAngle(sF, cF, Math::unitVecY) * Math::unitVecY;
  };

  //
  sk.userFn = [&]() {
    // sk.play = false;

    if (_jump) {
      if (changeSpeed) {
        sk.speed *= 0.5f;
        changeSpeed = false;
      }
      sk.setMotion("jump");
    } else {
      if (!changeSpeed) {
        sk.speed *= 2.0f;
        changeSpeed = true;
      }
      isMoving() ? sk.setMotion("walk") : sk.setMotion(staticMoName);
    }

    if (front) sk.transform.rot += angle(0.f) * rotSpeed;
    if (right) sk.transform.rot += angle(270.f) * rotSpeed;
    if (back) sk.transform.rot += angle(180.f) * rotSpeed;
    if (left) sk.transform.rot += angle(90.f) * rotSpeed;

    if (isMoving() || _jump) {
      sk.transform.pos += step();
      auto afs = abs(Settings::floorSize) * 0.95f;

      if (bool teleport = Settings::floorSize >= 500.f) {
        afs = abs(Settings::floorSize) * 0.35f;
        if (abs(sk.transform.pos.x) > afs) sk.transform.pos.x *= -1.f;
        if (abs(sk.transform.pos.z) > afs) sk.transform.pos.z *= -1.f;
      }
      sk.transform.pos = glm::clamp(sk.transform.pos, -afs, afs);
    }
  };

  // toggle play state
  sk.onKey(GLFW_KEY_0,
           [&]() { sk.play = !sk.play; },
           emptyFn,
           [&]() { sk.play = !sk.play; });

  // sk interaction
  sk.onKey(GLFW_KEY_SPACE, [&]() { _jump = true; }, [&]() { _jump = false; });
  sk.onKey(GLFW_KEY_W, [&]() { front = true; }, [&]() { front = false; });
  sk.onKey(GLFW_KEY_D, [&]() { right = true; }, [&]() { right = false; });
  sk.onKey(GLFW_KEY_S, [&]() { back = true; }, [&]() { back = false; });
  sk.onKey(GLFW_KEY_A, [&]() { left = true; }, [&]() { left = false; });

  // camera
  sk.onKey(GLFW_KEY_F, [&]() { sk.toggleCameraFollow(); });
  sk.onKey(GLFW_KEY_C, [&]() { sk.camera->resetOffset(); });

  // speed-control
  sk.onKey(
      GLFW_KEY_I, [&]() { sk.decSpeed(); }, emptyFn, [&]() { sk.decSpeed(); });
  sk.onKey(
      GLFW_KEY_O, [&]() { sk.incSpeed(); }, emptyFn, [&]() { sk.incSpeed(); });

  // linkalpha-control
  auto decAlpha = [&]() {
    sk.decLinkedAlpha();
    sk.decSpeed();
  };
  auto incAlpha = [&]() {
    sk.incLinkedAlpha();
    sk.incSpeed();
  };
  sk.onKey(GLFW_KEY_K, decAlpha, emptyFn, decAlpha);
  sk.onKey(GLFW_KEY_L, incAlpha, emptyFn, incAlpha);

  // un/link run motion to walk motion
  sk.onKey(GLFW_KEY_9,
           [&]() { walk->linked = (!walk->linked) ? run : nullptr; });

  // === ANIMATE ===
  sk.animate();
  sk.toggleCameraFollow();


  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------

  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto initFn = [&]() {
    auto _floor             = Renderable::getByName("Floor");
    _floor->transform.pos.y = sk.footHeight() - sk.transform.pos.y;
  };

  auto updateFn = [&]() {
    auto _floor             = Renderable::getByName("Floor");
    _floor->transform.scl.x = Settings::floorSize;
    _floor->transform.scl.z = Settings::floorSize;
  };

  auto renderFn = [&]() {
    sk.draw();

    camera->frame();
    Shader::poolUpdate(camera);
    Renderable::poolDraw(camera);

    static std::once_flag initFlag;
    std::call_once(initFlag, initFn);
  };

  // Init winow loop
  IO::windowVisibility(true); //! Do not remove. Avoid white screen on loading.
  IO::windowLoop(renderFn, updateFn);

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

  return 0;
}
