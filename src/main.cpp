#include "../incl/Core.hpp"
#include "../incl/Prefabs.hpp"
#include "../incl/Settings.hpp"
#include "../incl/Wrap/Paths.hpp"


int main(int argc, char const* argv[]) {

  BRAVE::Settings::init(BRAVE::Paths::Settings);
  BRAVE::Core::init();

  // ! Missing keyboard actions!!!

  auto floor = BRAVE::Prefab::plane(BRAVE::Colors::Cyan,
                                    glm::vec3{0.f, -18.f, 0.f},
                                    glm::vec3{0},
                                    glm::vec3{20.f, 20.f, 10.f});

  BRAVE::Core::onUpdate([&]() {
    BRAVE::Core::camera->frame();
    BRAVE::Core::draw(floor);
  });

  return 0;
}
