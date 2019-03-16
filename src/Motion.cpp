#include "Motion.hpp"

// #include <execution>
#include <numeric>
#include <tuple>

#include <dac/Logger.hpp>


namespace brave {

std::tuple<unsigned int, unsigned int>
    lowestErrFrames(const std::vector<Frame>& framesA,
                    const std::vector<Frame>& framesB = std::vector<Frame>()) {
  unsigned int f1, f2;

  if (framesA.empty() && framesB.empty()) {
    dInfo("At least one motion should be valid");
    return std::make_tuple(f1, f2);
  }


  if (framesB.empty()) {
    unsigned int numFramesA = framesA.size();
    unsigned int limitA     = numFramesA / 2.f;
    unsigned int limitB     = numFramesA - limitA;
    float        auxError   = std::numeric_limits<float>::max();

    for (auto i = 0u; i < limitA; ++i) {
      glm::vec3 iSum{0.f};
      for (const auto& r : framesA.at(i).rotations) { iSum += r; }
      for (auto ii = limitB; ii < numFramesA; ++ii) {
        glm::vec3 iiSum{0.f};
        for (const auto& r : framesA.at(ii).rotations) { iiSum += r; }

        auto currError = glm::compAdd(glm::abs(iSum - iiSum));
        if (auxError >= currError) {
          std::tie(auxError, f1, f2) = {currError, i, ii};
        }
      }
    }
  }

  return std::make_tuple(f1, f2);

  // ? mix
}

void Motion::clean() {
  // Get low errror frames of animation to generate a loop
  auto [I, E] = lowestErrFrames(this->frames);

  // Clean
  std::vector<Frame> auxFrames;
  auxFrames.reserve(E);
  for (auto f = I; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }

  //   for (auto i = 0u; i < auxFrames.size(); ++i) {
  //     auto Curr = auxFrames.at(i);
  //     auto Next = auxFrames.at((i + 1u < auxFrames.size()) ? i + 1u : 0u);
  //     for (auto alpha = 0.1f; alpha <= 0.9f; alpha += lerpFactor) {
  //       J->addRot(glm::mix(Curr, Next, alpha));
  //     }
  //   }

  // Smoother loop
  dInfo("tick:1");
  auto auxFramesLast = auxFrames.at(auxFrames.size() - 1);

  for (auto alpha = 0.1f; alpha <= 0.9f; alpha += 0.4f) {

    auto firstFrameRots = auxFrames.at(0).rotations;
    auto lastFrameRots  = auxFramesLast.rotations;
    assert(firstFrameRots.size() == lastFrameRots.size());

    Frame frame;
    frame.translation = auxFramesLast.translation; //* (1.2f * alpha);
    for (auto i = 0u; i < firstFrameRots.size(); ++i) {
      frame.rotations.push_back(
          glm::mix(firstFrameRots[i], lastFrameRots[i], alpha));
    }
    auxFrames.push_back(frame);
  }
  dInfo("tick:2");

  // Log
  auto d1 = this->frames.size();
  auto d2 = auxFrames.size();
  dInfo("Saved frames: {} - {} = {}", d1, d2, d1 - d2);

  // Store
  this->frames = auxFrames;
}


void Motion::mix(const std::shared_ptr<Motion>& m) {}

} // namespace brave
