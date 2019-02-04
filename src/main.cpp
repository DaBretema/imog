#include "Core.hpp"
#include "Renderable.hpp"
#include "Settings.hpp"
#include "Texture.hpp"

#include "helpers/Paths.hpp"
#include "helpers/Colors.hpp"

int main(int argc, char const* argv[]) {

  BRAVE::Settings::init(BRAVE::Paths::Settings);

  // BRAVE::Settings::dump();


  // std::this_thread::sleep_for(std::chrono::duration<float>(3.f));
  BRAVE::Core::init();

  // auto floor = BRAVE::Renderable(BRAVE::Paths::Figures + "plane.obj",
  //                                BRAVE::Paths::Textures + "floor.png");
  // floor.translate(0.f, -18.f, 0.f);
  // floor.scale(20.f, 20.f, 10.f);

  // auto cubeTex = BRAVE::Texture(BRAVE::Paths::Textures + "floor.png");

  auto lightMark = BRAVE::Renderable::create(
      BRAVE::Paths::Figures + "sphere.obj", "", BRAVE::Colors::Yellow);
  lightMark->translate(BRAVE::Core::light->pos());

  auto plane = BRAVE::Renderable::create(BRAVE::Paths::Figures + "plane.obj",
                                         BRAVE::Paths::Textures + "floor.png");
  plane->translate(0.f, -18.f, 0.f);
  plane->scale(20.f, 20.f, 10.f);

  BRAVE::Core::onUpdate([&]() { BRAVE::Core::frame(); });

  return 0;
}
