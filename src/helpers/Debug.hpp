
#include <memory>
#include "../gltools_Math.hpp"
#include "../gltools_Camera.hpp"
#include "../gltools_Renderable.hpp"

namespace imog {
namespace Debug {

  // Calling it on draw loop. Show a vector (as a Cylinder) on the 3D space.
  void vec(std::shared_ptr<imog::Camera> camera,
           glm::vec3                     P,
           glm::vec3                     color  = glm::vec3(1.f, 0.f, 0.f),
           glm::vec3                     center = glm::vec3(0.f)) {
    auto P1 = center + P * 0.5f;
    auto P2 = center + P * -0.5f;

    auto cyl = Renderable::line(P1, P2, 10.f);
    cyl->color(color);
    cyl->draw(camera);
  };

  // Calling it on draw loop. Show a point (as a Cube) on the 3D space.
  void point(std::shared_ptr<imog::Camera> camera,
             glm::vec3                     P,
             glm::vec3                     color  = glm::vec3(1.f, 0.f, 0.f),
             glm::vec3                     center = glm::vec3(0.f)) {
    auto box           = Renderable::getByName("Cube");
    box->transform.scl = glm::vec3(3.f);
    box->transform.pos = center + P;
    box->color(color);
    box->draw(camera);
  };

} // namespace Debug
} // namespace imog
