#include "LoaderBVH.hpp"

#include <regex>
#include <unordered_map>

#include <dac/Timer.hpp>
#include <dac/Strings.hpp>
#include <dac/Files.hpp>

#include "Math.hpp"
#include "Settings.hpp"

// ========================================================================= //
// ===================== BRAVE - Loader BVH *HELPERS* ====================== //
// ========================================================================= //


// ====================================================================== //
// ====================================================================== //
// To know where the parser is
// ====================================================================== //

enum struct _MODES { none, hierarchy, motion };

// ====================================================================== //
// ====================================================================== //
// Tokens order is gived by appear the function TYPE must have the same order
// ====================================================================== //

enum struct _TOKEN {
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

const std::unordered_map<std::string, unsigned int> _chMap{{"Xposition", 0u},
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

_TOKEN Token(const std::string& tokenStr) {
  bool         matched{false};
  unsigned int tokenValue{0u};

  for (auto TokenRE : _tokenREs) {
    tokenValue += 1;
    if (std::regex_match(tokenStr, TokenRE)) {
      matched = true;
      break;
    };
  }

  return (matched) ? static_cast<_TOKEN>(tokenValue) : _TOKEN::none;
};

// ====================================================================== //
// ====================================================================== //
// Add new joint to the joint list with the gived name
// ====================================================================== //

void addNewJoint(const std::string&                          name,
                 std::stack<std::shared_ptr<brave::Joint>>&  parents,
                 std::vector<std::shared_ptr<brave::Joint>>& joints) {

  auto j = std::make_shared<brave::Joint>(name);

  if (!parents.empty()) { j->parent(parents.top()); }
  parents.push(j);

  joints.push_back(j);
};

// ====================================================================== //
// ====================================================================== //
// For the gived line (called if is channel) iter and catch the channel info
// ====================================================================== //

void addChannelsOfCurrentLine(
    const std::vector<std::string>&            currLine,
    std::stack<std::shared_ptr<brave::Joint>>& parents) {

  for (auto i{2u}; i < currLine.size(); ++i) {
    parents.top()->addChannel(_chMap.at(currLine.at(i)));
  }
};

// ====================================================================== //
// ====================================================================== //
// A wrap for digest the data of each Motion slice line
// ====================================================================== //

void processMotionLine(const std::vector<std::string>&             currLine,
                       std::vector<std::shared_ptr<brave::Joint>>& joints) {

  glm::vec3 aux{0.f};
  auto      dataIdx{0u};
  auto      root{joints.at(0)};

  // Root translation // ! Iter positions 0,1,2.
  for (auto i : {0, 1, 2})
    aux[root->channels(i)] = std::stof(currLine.at(dataIdx++));
  root->addTrans(aux);

  // Root rotation  // ! Iter positions 3,4,5.
  for (auto i : {3, 4, 5})
    aux[root->channels(i)] = std::stof(currLine.at(dataIdx++));
  root->addRot(aux);

  // Other joints rotations // ! Iter positions 0,1,2.
  for (auto j = 1u; j < joints.size(); ++j) {
    auto J = joints.at(j);
    if (!J->channels().empty()) {
      for (auto i : {0, 1, 2})
        aux[J->channels(i)] = std::stof(currLine.at(dataIdx++));
      J->addRot(aux);
    }
  }
}



// ========================================================================= //
// ====================== BRAVE - Loader BVH *PARSER* ====================== //
// ========================================================================= //


namespace brave {
namespace loader {

  std::tuple<std::vector<std::shared_ptr<Joint>>, float>
      BVH(const std::string& bvhFilePath) {

    // Output vars
    std::vector<std::shared_ptr<Joint>> joints{};
    float                               frameTime{0.f};

    // File sanity check
    if (!dac::Files::ok(bvhFilePath, true)) {
      return std::make_tuple(joints, frameTime); // Returns empty
    }

    // --- AUX VARS ---------------------------------------------------- //
    // ----------------------------------------------------------------- //

    if (!Settings::quiet) dac::Timer timer("Parsing \"" + bvhFilePath + "\"");

    _MODES mode{_MODES::hierarchy};

    std::stack<std::shared_ptr<Joint>> parents{};

    std::string       linestream{""};
    std::stringstream filestream{dac::Strings::fromFile(bvhFilePath)};

    // ----------------------------------------------------------------- //
    // -------------------------------------------------- / AUX VARS --- //


    while (std::getline(filestream, linestream)) {
      auto LINE   = dac::Strings::split(linestream, " "); // ! First
      auto lToken = Token(LINE.at(0));                    // ! Second

      switch (mode) {

        // HIERARCHY
        case _MODES::hierarchy:
          switch (lToken) {

            case _TOKEN::braceR: parents.pop(); break;

            case _TOKEN::root: addNewJoint("Root", parents, joints); break;

            case _TOKEN::joint: addNewJoint(LINE.at(1), parents, joints); break;

            case _TOKEN::offset:
              parents.top()->offset(Math::glmVec3FromStr(LINE));
              break;

            case _TOKEN::channel:
              addChannelsOfCurrentLine(LINE, parents);
              break;

            case _TOKEN::endSite:
              addNewJoint("EndSite", parents, joints);
              break;

            case _TOKEN::hierarchy:
            default: continue; break;

            case _TOKEN::motion: mode = _MODES::motion; break;
          }
          break;

        // MOTION
        case _MODES::motion:
          switch (lToken) {

            case _TOKEN::motion:
            case _TOKEN::frameNum: break;

            case _TOKEN::frameTime: frameTime = std::stof(LINE.at(2u)); break;

            default: processMotionLine(LINE, joints); break;
          }
          break;

        // Default
        default: continue; break;
      }
    }

    return std::make_tuple(joints, frameTime);
  }

} // namespace loader
} // namespace brave
