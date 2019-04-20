#include "Loader.hpp"

#include <stack>
#include <regex>
#include <unordered_map>

#include "Math.hpp"
#include "Files.hpp"
#include "Timer.hpp"
#include "Logger.hpp"
#include "Strings.hpp"
#include "Settings.hpp"

// ========================================================================= //
// ===================== BRAVE - Loader BVH *HELPERS* ====================== //
// ========================================================================= //

// ====================================================================== //
// ====================================================================== //
// To know where the parser is
// ====================================================================== //

enum struct MODES { none, hierarchy, motion };

// ====================================================================== //
// ====================================================================== //
// Tokens order is gived by appear the function TYPE must have the same order
// ====================================================================== //

enum struct TOKEN {
  none,
  braceR,
  hierarchy,
  motion,
  root,
  joint,
  offset,
  channel,
  endSite,
  frameNum,
  frameTime
};

// ====================================================================== //
// ====================================================================== //
// Wrap strings to values (avoid tons of if-else at parsing)
// ====================================================================== //

const std::unordered_map<std::string, unsigned int> CHANMAP{{"Xposition", 0u},
                                                            {"Yposition", 1u},
                                                            {"Zposition", 2u},
                                                            {"Xrotation", 0u},
                                                            {"Yrotation", 1u},
                                                            {"Zrotation", 2u}};

// ====================================================================== //
// ====================================================================== //
// Regular expresion of posible token strings
// ====================================================================== //

const std::array<std::regex, 10> _tokenREs{std::regex(".*\\}"),
                                           std::regex(".*HIERARCHY"),
                                           std::regex(".*MOTION"),
                                           std::regex(".*ROOT"),
                                           std::regex(".*JOINT"),
                                           std::regex(".*OFFSET"),
                                           std::regex(".*CHANNELS"),
                                           std::regex(".*End"),
                                           std::regex(".*Frames:"),
                                           std::regex(".*Frame")};

// ====================================================================== //
// ====================================================================== //
// Get token enum from a string
// ====================================================================== //

TOKEN Token(const std::string& tokenStr) {
  bool         matched{false};
  unsigned int tokenValue{0u};

  for (auto TokenRE : _tokenREs) {
    tokenValue += 1;
    if (std::regex_match(tokenStr, TokenRE)) {
      matched = true;
      break;
    };
  }

  return (matched) ? static_cast<TOKEN>(tokenValue) : TOKEN::none;
};

// ====================================================================== //
// ====================================================================== //
// Add new joint to the joint list with the gived name
// ====================================================================== //

void addNewJoint(const std::string&                          name,
                 std::stack<std::shared_ptr<brave::Joint>>&  parents,
                 std::vector<std::shared_ptr<brave::Joint>>& joints,
                 bool isEndSite = false) {

  auto j = std::make_shared<brave::Joint>(
      name, (!parents.empty()) ? parents.top() : nullptr);

  parents.push(j);
  if (!isEndSite) { joints.push_back(j); }
  if (isEndSite && j->parent) { j->parent->endsite = j; }
  if (isEndSite && !j->parent) { LOGE("An EndSite, should have parent!") }
};



// ========================================================================= //
// ====================== BRAVE - Loader BVH *PARSER* ====================== //
// ========================================================================= //


namespace brave {
namespace loader {

  std::shared_ptr<Motion>
      BVH(const std::string& bvhFilePath, loopMode lm, unsigned int steps) {
    auto out = std::make_shared<Motion>();
    if (!Files::ok(bvhFilePath, true)) { return out; }

    // --- AUX VARS ---------------------------------------------------- //
    // ----------------------------------------------------------------- //

    if (!Settings::quiet) Timer timer("Parsing \"" + bvhFilePath + "\"");

    MODES mode{MODES::hierarchy};

    std::stack<std::shared_ptr<brave::Joint>> parents{};
    std::vector<unsigned int>                 channels; // Read order for motion

    std::string       linestream{""};
    std::stringstream filestream{Strings::fromFile(bvhFilePath)};

    // ----------------------------------------------------------------- //
    // -------------------------------------------------- / AUX VARS --- //

    while (std::getline(filestream, linestream)) {
      auto LINE   = Strings::split(linestream, " "); // ! First
      auto lToken = Token(LINE.at(0));               // ! Second

      switch (mode) {

        // HIERARCHY
        case MODES::hierarchy:
          switch (lToken) {

            case TOKEN::braceR: parents.pop(); break;

            case TOKEN::root: addNewJoint("Root", parents, out->joints); break;

            case TOKEN::joint:
              addNewJoint(LINE.at(1), parents, out->joints);
              break;

            case TOKEN::offset:
              parents.top()->offset = Math::glmVec3FromStr(LINE);
              break;

            case TOKEN::channel:
              for (auto i{2u}; i < LINE.size(); ++i)
                channels.push_back(CHANMAP.at(LINE.at(i)));
              break;

            case TOKEN::endSite:
              addNewJoint("EndSite", parents, out->joints, true);
              break;

            case TOKEN::hierarchy:
            default: continue; break;

            case TOKEN::motion: mode = MODES::motion; break;
          }
          break;

        // MOTION
        case MODES::motion:
          switch (lToken) {

            case TOKEN::motion:
            case TOKEN::frameNum: break;

            case TOKEN::frameTime:
              out->timeStep = std::stof(LINE.at(2u));
              break;

            default:
              brave::Frame currFrame;
              glm::vec3    aux{0.f};

              assert(LINE.size() == channels.size());

              // Translation
              for (auto i : {0, 1, 2})
                aux[channels.at(i)] = std::stof(LINE.at(i));
              currFrame.translation = aux;

              // Rotations
              for (auto i = 3u; i < LINE.size(); i += 3) {
                for (auto j = i; j < i + 3; ++j) {
                  aux[channels.at(j)] = std::stof(LINE.at(j));
                }
                currFrame.rotations.push_back(aux);
              }

              out->frames.push_back(currFrame);
              break;
          }
          break;

        // Default
        default: continue; break;
      }
    }

    out->clean(lm, steps);
    return out;
  }
} // namespace loader
} // namespace brave
