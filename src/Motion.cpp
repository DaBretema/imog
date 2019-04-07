#include "Motion.hpp"

#include <tuple>
#include <numeric>

#include "Logger.hpp"
#include "Settings.hpp"


namespace brave {

// * helpers


// ====================================================================== //
// ====================================================================== //
// Error processing function (to avoid copy-paste failures because
// two cases) are so similar
// ====================================================================== //

void updateErr(float&          auxErr,
               uint&           f1,
               uint&           f2,
               uint            a,
               const glm::vec3 aSum,
               uint            b,
               glm::vec3       bSum) {
  auto err = glm::compAdd(glm::abs(aSum - bSum));
  if (auxErr >= err) { std::tie(auxErr, f1, f2) = {err, a, b}; }
}

// ====================================================================== //
// ====================================================================== //
// Get lowest err frames in ONE motion
// ====================================================================== //

std::tuple<uint, uint> lowestErrFrames_1(const std::vector<Frame>& frames) {
  uint  f1, f2;
  uint  nFrames = frames.size();
  uint  limitA  = nFrames / 2.f;
  uint  limitB  = nFrames - limitA;
  float auxErr  = std::numeric_limits<float>::max();

  for (auto a = 0u; a < limitA; ++a) {
    glm::vec3 aSum = frames.at(a).sumRots();
    for (auto b = limitB; b < nFrames; ++b) {
      glm::vec3 bSum = frames.at(b).sumRots();
      updateErr(auxErr, f1, f2, a, aSum, b, bSum);
    }
  }

  return std::make_tuple(f1, f2);
}

// ====================================================================== //
// ====================================================================== //
// Get lowest err frames of TWO motions
// ====================================================================== //

std::tuple<uint, uint> lowestErrFrames_2(const std::vector<Frame>& framesA,
                                         const std::vector<Frame>& framesB) {
  uint  f1, f2;
  float auxErr = std::numeric_limits<float>::max();

  for (auto a = 0u; a < framesA.size(); ++a) {
    glm::vec3 aSum = framesA.at(a).sumRots();
    for (auto b = 0u; b < framesB.size(); ++b) {
      glm::vec3 bSum = framesB.at(b).sumRots();
      updateErr(auxErr, f1, f2, a, aSum, b, bSum);
    }
  }

  return std::make_tuple(f1, f2);
}


// * class methods


// ====================================================================== //
// ====================================================================== //
// Sum rotations of frame
// ====================================================================== //

glm::vec3 Frame::sumRots() const {
  glm::vec3 sum{0.f};
  for (const auto& r : this->rotations) { sum += r; }
  return sum;
}

// ====================================================================== //
// ====================================================================== //
// If its name contains _ is a mix
// ====================================================================== //
bool Motion::isMix(const std::string& str) {
  return str.find("_") != std::string::npos;
}
bool Motion::isMix() { return isMix(this->name); }

// ====================================================================== //
// ====================================================================== //
// Clean any motion to get a smoother loop
// ====================================================================== //

void Motion::clean(bool makeLoop) {
  this->frames.erase(this->frames.begin());
  if (!makeLoop) return;

  // Get low errror frames of animation to generate a loop
  auto [I, E] = lowestErrFrames_1(this->frames);
  // Clean
  std::vector<Frame> auxFrames;
  auxFrames.reserve(E);
  for (auto f = I; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }

  // LERP
  /* ... */

  // Store
  this->frames = auxFrames;
}

// ====================================================================== //
// ====================================================================== //
// Mix any motion with other and get a new animation
// that conect both smoothly
// ====================================================================== //

std::shared_ptr<Motion> Motion::mix(const std::shared_ptr<Motion>& m2) {
  auto [LEF1, LEF2] = lowestErrFrames_2(this->frames, m2->frames);

  auto M = std::make_shared<Motion>();

  M->frameA      = LEF1;
  M->frameB      = LEF2;
  M->joints      = this->joints;
  M->lockRotOnXZ = (this->lockRotOnXZ || m2->lockRotOnXZ);
  M->timeStep    = (this->timeStep + m2->timeStep) * 0.5f;

  auto F1 = this->frames.at(LEF1);
  auto F2 = m2->frames.at(LEF2);
  assert(F1.rotations.size() == F2.rotations.size());

  for (auto alpha = 0.f; alpha <= 1.f; alpha += 0.01f) {
    Frame frame;

    // Translation // ?
    auto T            = F1.translation;
    auto TLerp        = glm::mix(F1.translation, F2.translation, alpha);
    frame.translation = glm::vec3(T.x, TLerp.y, T.z);

    // Rotations
    for (auto i = 0u; i < F1.rotations.size(); ++i) {
      auto newRot = glm::mix(F1.rotations[i], F2.rotations[i], alpha);
      // Avoid X and Z rotations of root to avoid visual bugs
      if (i == 0 /*&& M->lockRotOnXZ*/) {
        newRot.y = (newRot.x + newRot.z) * 0.5f - newRot.y;
        newRot.x = 0;
        newRot.z = 0;
      }
      frame.rotations.push_back(newRot);
    }

    // Store frame
    M->frames.push_back(frame);
  }

  return M;
}

} // namespace brave
