#include "Motion.hpp"

#include <tuple>
#include <numeric>

#include "Logger.hpp"
#include "Loader.hpp"
#include "Settings.hpp"


namespace brave {

// * helpers

#define __ERR_UPDATE()                            \
  auto err = glm::compAdd(glm::abs(aSum - bSum)); \
  if (auxErr >= err) {                            \
    std::tie(auxErr, f1, f2) = {err, a, b};       \
  }

// ====================================================================== //
// ====================================================================== //
// Get lowest err frames in ONE motion
// ====================================================================== //

std::tuple<uint, uint> lowestErrFrames(const std::vector<Frame>& frames) {
  uint  f1, f2;
  uint  nFrames = frames.size();
  uint  limitA  = nFrames / 2.f;
  uint  limitB  = nFrames - limitA;
  float auxErr  = std::numeric_limits<float>::max();

  for (auto a = 0u; a < limitA; ++a) {
    glm::vec3 aSum = frames.at(a).sumRots();
    for (auto b = limitB; b < nFrames; ++b) {
      glm::vec3 bSum = frames.at(b).sumRots();
      __ERR_UPDATE();
    }
  }

  return std::make_tuple(f1, f2);
}

// ====================================================================== //
// ====================================================================== //
// Get lowest err frames of TWO motions
// ====================================================================== //

std::tuple<uint, uint> lowestErrFrames(const std::vector<Frame>& framesA,
                                       const std::vector<Frame>& framesB) {
  uint  f1, f2;
  float auxErr = std::numeric_limits<float>::max();

  for (auto a = 0u; a < framesA.size(); ++a) {
    glm::vec3 aSum = framesA.at(a).sumRots();
    for (auto b = 0u; b < framesB.size(); ++b) {
      glm::vec3 bSum = framesB.at(b).sumRots();
      __ERR_UPDATE();
    }
  }

  return std::make_tuple(f1, f2);
}

// ====================================================================== //
// ====================================================================== //
// Transform rot to y-rot
// ====================================================================== //

glm::vec3 yRot(const glm::vec3& rot, float dir = 1.0f) {
  auto Y = (rot.x + rot.z) * 0.5f - rot.y;
  return glm::vec3{0.0f, Y * dir, 0.0f};
}

// ====================================================================== //
// ====================================================================== //
// Interpolate between two frames
// ====================================================================== //

std::vector<Frame>
    lerp(Frame F1, Frame F2, uint steps = 0u, bool lerpT = true) {
  assert(F1.rotations.size() == F2.rotations.size());

  std::vector<Frame> lerpFrames;

  if (steps < 1)
    return lerpFrames;
  else if (steps < 2)
    steps = 2;

  float alphaStep = 1.0f / (float)steps;

  for (auto alpha = 0.0f; alpha <= 1.0f; alpha += alphaStep) {
    Frame frame;
    // Translation
    frame.translation = (lerpT)
                            ? glm::mix(F1.translation, F2.translation, alpha)
                            : F1.translation;
    // Rotations
    for (auto i = 0u; i < F1.rotations.size(); ++i) {
      auto newRot = glm::mix(F1.rotations[i], F2.rotations[i], alpha);
      // if (i == 0u) { newRot = yRot(newRot); } // Avoid X & Z root rotations
      frame.rotations.push_back(newRot);
    }
    // Store frame
    lerpFrames.push_back(frame);
  }
  return lerpFrames;
}



// * Frame methods

// ====================================================================== //
// ====================================================================== //
// Sum rotations of frame
// ====================================================================== //

glm::vec3 Frame::sumRots() const {

  // * V1 all joints weighs the same
  // glm::vec3 sum{0.0f};
  // for (const auto& r : this->rotations) { sum += r; }
  // return sum;

  // * V2 root joint weigh the double that the rest of joints
  auto sum = this->rotations.at(0) * 2.0f; //! awesome improvement
  for (auto i = 1u; i < this->rotations.size(); i++) {
    sum += this->rotations.at(i);
  }

  return sum;
}



// * Motion methods

// ====================================================================== //
// ====================================================================== //
// Constructor
// ====================================================================== //

std::shared_ptr<Motion> Motion::create(const std::string& name,
                                       const std::string& filepath,
                                       loopMode           lm,
                                       uint               steps) {
  auto m = loader::BVH(filepath);
  m->clean(lm, steps);
  m->name = name;
  return m;
}

// ====================================================================== //
// ====================================================================== //
// If its name contains _ is a mix
// ====================================================================== //

bool Motion::isMix(const std::string& str) { // ! static
  return str.find("_") != std::string::npos;
}

bool Motion::isMix() { return isMix(this->name); }

// ====================================================================== //
// ====================================================================== //
// Clean any motion to get a smoother loop
// ====================================================================== //

void Motion::clean(loopMode lm, uint steps) {
  this->frames.erase(this->frames.begin());
  if (lm == loopMode::none) return;

  // Get low errror frames of animation to generate a loop
  auto [I, E] = lowestErrFrames(this->frames);

  // Clean
  if (lm == loopMode::shortLoop) {

    std::vector<Frame> auxFrames;
    auxFrames.reserve(E);
    for (auto f = I; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }

    this->frames = auxFrames; // Store
  }

  // Lerp
  auto first = this->frames.front();
  auto last  = this->frames.back();
  for (const auto& F : lerp(last, first, steps, false)) {
    this->frames.push_back(F);
  }
}

// ====================================================================== //
// ====================================================================== //
// Mix any motion with other and get a new animation
// that conect both smoothly
// ====================================================================== //

std::shared_ptr<Motion> Motion::mix(const std::shared_ptr<Motion>& m2) {
  auto [LEF1, LEF2] = lowestErrFrames(this->frames, m2->frames);

  auto M      = std::make_shared<Motion>();
  M->frameA   = LEF1;
  M->frameB   = LEF2;
  M->joints   = this->joints;
  M->timeStep = (this->timeStep + m2->timeStep) * 0.5f;

  auto fM1 = this->frames.at(LEF1);
  auto fM2 = m2->frames.at(LEF2);
  for (const auto& F : lerp(fM1, fM2, 50u, true)) { M->frames.push_back(F); }

  return M;
}

} // namespace brave
