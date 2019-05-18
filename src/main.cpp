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

  auto jump = Motion::create("jump", Motions::jump, loopMode::shortLoop, 10u);
  sk.onKey(GLFW_KEY_SPACE,
           [&]() {
             sk.setMotion("jump");
             sk.allowedTrans = Math::unitVecY; // Allow jump
           },
           [&]() {
             //sk.loadPrevMotion();
             sk.setMotion("walk");
             sk.allowedTrans = Math::nullVec; // Block jump
           });
  sk.addMotion(jump);

  auto walk      = Motion::create("walk", Motions::walk, loopMode::shortLoop);
  bool walking   = false;
  auto startWalk = [&]() {
    sk.setMotion("walk");
    walking = true;
  };
  auto stopWalk = [&]() {
    sk.setMotion("idle");
    walking = false;
  };
  sk.addMotion(walk);

  // auto run       = Motion::create("run", Motions::run, loopMode::shortLoop);
  // bool toggleRun = false;
  // sk.onKey(GLFW_KEY_R, [&]() {
  //   toggleRun = !toggleRun;
  //   (toggleRun) ? sk.setMotion("run")
  //               : (walking) ? sk.setMotion("walk") : sk.setMotion("idle");
  // });
  // sk.addMotion(run);

  auto idle = Motion::create("idle", Motions::tPose, loopMode::loop, 25u);
  sk.addMotion(idle);



  sk.onKey(GLFW_KEY_0, [&]() { sk.play = !sk.play; });

  {
    auto step = [&]() {
      sk.transform.pos += sk.transform.front() * sk.step() * Math::vecXZ;
    };

    auto angle = [&](float rotAngle) {
      step();

      auto sF = sk.transform.front() * Math::vecXZ;
      auto cF = glm::rotateY(camera->pivot.right(), glm::radians(90.f));
      cF      = glm::rotateY(cF * Math::vecXZ, glm::radians(rotAngle));

      auto angle = glm::angle(sF, cF) * 2.f;
      auto cross = glm::cross(sF, cF);
      auto dot   = glm::dot(cross, Math::unitVecY);


      return Math::unitVecY * ((dot < 0) ? -angle : angle);
    };

    auto goF = [&]() { sk.transform.rot += angle(0.f); };
    auto goR = [&]() { sk.transform.rot += angle(-90.f); };
    auto goB = [&]() { sk.transform.rot += angle(180.f); };
    auto goL = [&]() { sk.transform.rot += angle(90.f); };

    sk.onKey(GLFW_KEY_W,
             [&]() {
               goF();
               startWalk();
             },
             stopWalk,
             goF);
    sk.onKey(GLFW_KEY_D,
             [&]() {
               goR();
               startWalk();
             },
             stopWalk,
             goR);
    sk.onKey(GLFW_KEY_S,
             [&]() {
               goB();
               startWalk();
             },
             stopWalk,
             goB);
    sk.onKey(GLFW_KEY_A,
             [&]() {
               goL();
               startWalk();
             },
             stopWalk,
             goL);
  }
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

    // if (sk.transform.pos.y < 0.f) sk.transform.pos.y = 0.f;
    // if (sk.transform.pos.y > 1.f) sk.transform.pos.y = 1.f;

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
