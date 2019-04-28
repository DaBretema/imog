#include "Loader.hpp"

#include <mutex>
#include <sstream>
#include <unordered_map>

#include "Files.hpp"
#include "Logger.hpp"
#include "Strings.hpp"

namespace brave {
namespace loader {

  // ====================================================================== //
  // ====================================================================== //
  // Wrap the process to load a 3D obj from OBJ file
  // ====================================================================== //

  Renderable::data OBJ(const std::string& filePath) {
    Renderable::data out;
    if (!Files::ok(filePath, true)) { return out; }

    // --- AUX VARS ---------------------------------------------------- //
    // ----------------------------------------------------------------- //

    // UVs vars
    unsigned int                           uvIdx;
    std::vector<glm::vec2>                 uvsTemp;
    std::unordered_map<unsigned int, bool> uvsCache;
    std::once_flag                         uvsOnceFlag;
    auto resizeUVs = [&]() { out.uvs.resize(out.vertices.size()); };


    // Normals vars
    unsigned int                           nIdx;
    std::vector<glm::vec3>                 normsTemp;
    std::unordered_map<unsigned int, bool> normsCache;
    std::once_flag                         normsOnceFlag;
    auto resizeNorms = [&]() { out.normals.resize(out.vertices.size()); };

    // Faces vars
    std::vector<std::string> face;
    unsigned int             vIdx;

    // Str vars
    std::string       linestream;
    std::stringstream filestream(Strings::fromFile(filePath));

    // ----------------------------------------------------------------- //
    // -------------------------------------------------- / AUX VARS --- //


    while (std::getline(filestream, linestream)) {
      auto LINE   = Strings::split(linestream, " "); // ! First
      auto lToken = LINE.at(0);                      // ! Second

      if ((lToken == "#") || (lToken == "s")) { continue; }
      if (lToken == "v") { out.vertices.push_back(Math::glmVec3FromStr(LINE)); }
      if (lToken == "vt") { uvsTemp.push_back(Math::glmVec2FromStr(LINE)); }
      if (lToken == "vn") { normsTemp.push_back(Math::glmVec3FromStr(LINE)); }

      if (lToken == "f") {
        bool thereIsUVs   = uvsTemp.size() > 0;
        bool thereIsNorms = normsTemp.size() > 0;

        // ReSize to allow the use of [] operator
        if (thereIsUVs) std::call_once(uvsOnceFlag, resizeUVs);
        if (thereIsNorms) std::call_once(normsOnceFlag, resizeNorms);

        // Process vertex, normals and UVs indices
        for (auto i = 1u; i < LINE.size(); ++i) {

          if (thereIsUVs && thereIsNorms) {
            face  = Strings::split(LINE.at(i), "/");
            uvIdx = std::stoul(face.at(1)) - 1u;
            nIdx  = std::stoul(face.at(2)) - 1u;
          } else if (!thereIsUVs && thereIsNorms) {
            face = Strings::split(LINE.at(i), "//");
            nIdx = std::stoul(face.at(1)) - 1u;
          } else if (thereIsUVs && !thereIsNorms) {
            face  = Strings::split(LINE.at(i), "/");
            uvIdx = std::stoul(face.at(1)) - 1u;
          } else {
            LOGE("[WARN] Undefined Normals and UVs @ {}", filePath);
          }
          vIdx = std::stoul(face.at(0)) - 1u;

          // Fit on vertex index the assigned norm and uv
          if (thereIsUVs) out.uvs[vIdx] = uvsTemp.at(uvIdx);
          if (thereIsNorms) out.normals[vIdx] = normsTemp.at(nIdx);

          // Store vertex index
          out.indices.push_back(vIdx);
        }
      }
    }
    return out;
  }

} // namespace loader
} // namespace brave
