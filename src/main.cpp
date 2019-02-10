#include "Core.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"
#include "Joint.hpp"

#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"

#include <thread>
#include <chrono>

using namespace brave;

int main(int argc, char const* argv[]) {
  Core::init(Paths::settings);

  auto j1 = Joint("test");

  Core::onUpdate([&]() {
    Core::frame();
    j1.draw();
  });
  return 0;
}
