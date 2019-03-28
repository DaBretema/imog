#include "Motion.hpp"

#include <tuple>
#include <numeric>

#include "Logger.hpp"
#include "Settings.hpp"


namespace brave {
using uint = unsigned int;


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
// Clean any motion to get a smoother loop
// ====================================================================== //

void Motion::clean(loopMode lm) {

  auto cleanFirstFrame = [&]() { this->frames.erase(this->frames.begin()); };

  switch (lm) {
    default:
    case loopMode::firstFrame: cleanFirstFrame(); break;

    case loopMode::cycle: {
      // Get low errror frames of animation to generate a loop
      auto [I, E] = lowestErrFrames_1(this->frames);
      // Clean
      std::vector<Frame> auxFrames;
      auxFrames.reserve(E);
      for (auto f = I; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }
      // Smoother loop
      // for (auto alpha = 0.1f; alpha <= 0.9f; alpha += 0.4f) {
      //   auto firstRot = auxFrames.front().rotations;
      //   auto lastRot  = auxFrames.back().rotations;
      //   assert(firstRot.size() == lastRot.size());
      //   Frame frame;
      //   frame.translation = auxFrames.back().translation; //* (1.2f * alpha);
      //   for (auto i = 0u; i < firstRot.size(); ++i) {
      //     frame.rotations.push_back(glm::mix(lastRot[i], firstRot[i], alpha));
      //   }
      //   auxFrames.push_back(frame);
      // }
      // Store
      this->frames = auxFrames;
    } break;

    case loopMode::mirror: {
      cleanFirstFrame();
      auto framesN = this->frames.size();
      for (auto i = 0; i < framesN; ++i) {
        auto idx   = framesN - 1 - i;
        auto frame = this->frames.at(idx);
        this->frames.push_back(frame);
      }
    } break;
  }



  // // Log
  // auto d1 = this->frames.size();
  // auto d2 = auxFrames.size();
  // if (!Settings::quiet) LOGD("Saved frames: {} - {} = {}", d1, d2, d1 - d2);
}

// ====================================================================== //
// ====================================================================== //
// Mix any motion with other and get a new animation
// that conect both smoothly
// ====================================================================== //

std::shared_ptr<Motion> Motion::mix(const std::shared_ptr<Motion>& m2) {
  auto [lefM1, lefM2] = lowestErrFrames_2(this->frames, m2->frames);

  auto M    = std::make_shared<Motion>();
  M->joints = this->joints;

  // ? On that way, or 'frame by frame'
  M->timeStep = (this->timeStep + m2->timeStep) * 2.f;

  M->frames.push_back(this->frames.at(lefM1));

  auto M1Rots = this->frames.at(lefM1).rotations;
  auto M2Rots = m2->frames.at(lefM2).rotations;
  assert(M1Rots.size() == M2Rots.size());

  //DEBUG
  // LOG("M1")
  // for (auto i = 0u; i < M1Rots.size(); ++i) {
  //   LOG("{}", glm::to_string(M1Rots[i]));
  // }
  // LOG("\nM2")
  // for (auto i = 0u; i < M2Rots.size(); ++i) {
  //   LOG("{}", glm::to_string(M2Rots[i]));
  // }
  ///DEBUG

  for (auto alpha = 0.1f; alpha <= 0.9f; alpha += 0.05f) {
    // for (auto alpha = 0.1f; alpha <= 0.2f; alpha += 0.1f) {
    // LOG("Frame: {}", alpha);
    Frame frame;
    frame.translation = glm::vec3(0.f); //this->frames.at(lefM1).translation;
    for (auto i = 0u; i < M1Rots.size(); ++i) {
      auto newRot = glm::mix(M1Rots[i], M2Rots[i], alpha);
      // LOG("{}", glm::to_string(newRot));
      frame.rotations.push_back(newRot);
    }
    // LOG("\n\n");
    M->frames.push_back(frame);
  }

  M->frames.push_back(m2->frames.at(lefM2));

  return M;
}

} // namespace brave
