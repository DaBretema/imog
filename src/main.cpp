#include "../incl/Core.hpp"
#include "../incl/Prefabs.hpp"


int main(int argc, char const* argv[]) {

  auto floor = BRAVE::Prefab::plane(BRAVE::Colors::Cyan,
                                    glm::vec3{0.f, -18.f, 0.f},
                                    glm::vec3{0},
                                    glm::vec3{20.f, 20.f, 10.f});

  BRAVE::Core::onUpdate([&]() {



  });

  return 0;
}
