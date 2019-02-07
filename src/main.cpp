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
  Renderable::create("Plane", Figures::plane, Textures::chess);
  Core::onUpdate([&]() { Core::frame(); });
  return 0;
}
