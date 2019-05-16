#include "Motion.hpp"

#include <tuple>
#include <numeric>
#include <fstream>
#include <iostream>

#include "Logger.hpp"
#include "Loader.hpp"
#include "Settings.hpp"


namespace brave {

// * Helpers

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

  glm::vec3 rootRot{0.f};
  {
    auto fv1 = Math::rotToVec(F1.rotations[0]);
    auto fv2 = Math::rotToVec(F2.rotations[0]);
    rootRot  = Math::dirAngle(fv1, fv2) * Math::unitVecY;
  }

  for (auto alpha = 0.1f; alpha <= 1.0f; alpha += alphaStep) {
    Frame frame;

    auto newTrans     = glm::mix(F1.translation, F2.translation, alpha);
    frame.translation = (lerpT) ? newTrans : F1.translation;

    frame.rotations.push_back(F1.rotations[0] + alpha * rootRot);
    for (auto i = 1u; i < F1.rotations.size(); ++i) {
      auto newRot = glm::mix(F1.rotations[i], F2.rotations[i], alpha);
      frame.rotations.push_back(newRot);
    }

    lerpFrames.push_back(frame);
  }

  return lerpFrames;
}



// * Frame methods

// ====================================================================== //
// ====================================================================== //
// Sum rotations of frame
// ====================================================================== //

glm::vec3 Frame::value() const {

  // * V1 all joints weighs the same
  glm::vec3 sum{0.0f};
  for (const auto& r : this->rotations) { sum += r; }
  return sum;

  // * V2 root joint weigh the double that the rest of joints
  // auto sum = this->rotations.at(0) * 2.0f;
  // for (auto i = 1u; i < this->rotations.size(); i++) {
  //   sum += this->rotations.at(i);
  // }

  // * V3 root joint height substracts value
  // glm::vec3 sum{0.f};
  // for (auto i = 1u; i < this->rotations.size(); i++) {
  //   sum += this->rotations.at(i);
  // }
  // sum -= glm::abs(this->translation * Math::unitVecY); // ?

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

bool Motion::isMix() { return this->name.find("_") != std::string::npos; }

// ====================================================================== //
// ====================================================================== //
// Clean any motion to get a smoother loop
// ====================================================================== //

void Motion::clean(loopMode lm, uint steps) {
  this->frames.erase(this->frames.begin());
  if (lm == loopMode::none) return;

  // Get low errror frames of animation to generate a loop
  uint B, E;
  {
    uint  nFrames = frames.size();
    uint  limitA  = nFrames / 2.f;
    uint  limitB  = nFrames - limitA;
    float auxDiff = std::numeric_limits<float>::max();

    for (auto f1 = 0u; f1 < limitA; ++f1) {
      glm::vec3 f1Val = this->frames.at(f1).value();

      for (auto f2 = limitB; f2 < nFrames; ++f2) {
        glm::vec3 f2Val = this->frames.at(f2).value();

        auto diff = glm::compAdd(glm::abs(f1Val - f2Val));
        if (diff < auxDiff) { std::tie(auxDiff, B, E) = {diff, f1, f2}; }
      }
    }
  }

  // Clean
  if (lm == loopMode::shortLoop) {
    std::vector<Frame> auxFrames;
    auxFrames.reserve(E);
    for (auto f = B; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }
    this->frames = auxFrames; // Store
  }

  // Lerp
  auto first = this->frames.front();
  auto last  = this->frames.back();
  for (const auto& nF : lerp(last, first, steps, false)) {
    this->frames.push_back(nF);
  }
}

// ====================================================================== //
// ====================================================================== //
// Mix any motion with other and get a new animation
// that conect both smoothly
// ====================================================================== //

Motion::mixMap Motion::mix(const std::shared_ptr<Motion>& m2) {

  auto createTransitionMotion = [&](uint idxF1, uint idxF2) {
    auto mo      = std::make_shared<Motion>();
    mo->joints   = this->joints;
    mo->timeStep = (this->timeStep + m2->timeStep) * 0.5f;

    auto f1 = this->frames.at(idxF1);
    auto f2 = m2->frames.at(idxF2);
    for (const auto& nF : lerp(f1, f2, 50u, true)) { mo->frames.push_back(nF); }

    return mo;
  };

  //---

  // for heat map visualization
  std::ofstream heatmap;
  heatmap.open("./2plot/" + this->name + "_" + m2->name + "__heatmap.txt");
  std::ofstream refFrames;
  refFrames.open("./2plot/" + this->name + "_" + m2->name + "__refFrames.txt");
  std::ofstream diffPerFrame;
  diffPerFrame.open("./2plot/" + this->name + "_" + m2->name +
                    "__diffPerFrame.txt");
  //


  mixMap mm;
  uint   _f2   = 0;
  float  _diff = std::numeric_limits<float>::infinity();

  for (auto f1 = 0u; f1 < this->frames.size(); f1++) {
    _diff             = std::numeric_limits<float>::infinity(); // ! reset
    glm::vec3 f1Value = this->frames.at(f1).value();

    for (auto f2 = 0u; f2 < m2->frames.size(); f2++) {
      glm::vec3 f2Value = m2->frames.at(f2).value();

      float diff = glm::compAdd(glm::abs(f1Value - f2Value));
      if (diff < _diff) { std::tie(_diff, _f2) = {diff, f2}; }

      (f2 < m2->frames.size() - 1) ? heatmap << diff << " "
                                   : heatmap << diff << "\n";
    }
    refFrames << f1 << " " << _f2 << "\n";

    auto tMo = createTransitionMotion(f1, _f2);
    mm.insert({f1, {_f2, tMo}});
  }

  return mm;
}

} // namespace brave
