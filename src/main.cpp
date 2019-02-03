#include "Core.hpp"
#include "Renderable.hpp"
#include "Settings.hpp"
#include "Texture.hpp"

#include "helpers/Paths.hpp"
#include "helpers/Colors.hpp"


int main(int argc, char const* argv[]) {

  BRAVE::Settings::init(BRAVE::Paths::Settings);
  BRAVE::Core::init();

  // auto floor = BRAVE::Renderable(BRAVE::Paths::Figures + "plane.obj",
  //                                BRAVE::Paths::Textures + "floor.png");
  // floor.translate(0.f, -18.f, 0.f);
  // floor.scale(20.f, 20.f, 10.f);

  // auto cubeTex = BRAVE::Texture(BRAVE::Paths::Textures + "floor.png");

  auto cube = BRAVE::Renderable::create(BRAVE::Paths::Figures + "cube.obj",
                                        BRAVE::Paths::Textures + "floor.png");
  // cube.translate(0.f, -18.f, 0.f);
  // cube.scale(20.f, 20.f, 10.f);

  BRAVE::Core::onUpdate([&]() {
    BRAVE::Core::frame();
    // cube->translate(1.f, 1.f, 0.f);
    // cube->draw();
  });

  return 0;
}
