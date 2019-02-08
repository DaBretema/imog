#include "Core.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"

#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"

#include <thread>
#include <chrono>

using namespace brave;

int main(int argc, char const* argv[]) {
  Core::init(Paths::settings);

  auto plane = Renderable::create("Plane", Figures::plane, Textures::chess);
  plane->scl(100.f, 1.f, 100.f);

  Core::onUpdate([&]() { Core::frame(); });

  return 0;
}
