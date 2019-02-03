#include "LoaderOBJ.hpp"

#include <mutex>
#include <sstream>
#include <unordered_map>

#include <dac/Logger.hpp>
#include <dac/Strings.hpp>


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Wrap the process to load a 3D obj fromo OBJ file
// ====================================================================== //

RenderData loadOBJ(const std::string& filePath) {

  // --- AUX VARS ---------------------------------------------------- //
  // ----------------------------------------------------------------- //

  RenderData out;

  // Normals vars
  std::once_flag                         normsFlag;
  std::vector<glm::vec3>                 normsTemp;
  std::unordered_map<unsigned int, bool> normsCache;

  // UVs vars
  std::once_flag                         uvsFlag;
  std::vector<glm::vec2>                 uvsTemp;
  std::unordered_map<unsigned int, bool> uvsCache;

  // Str vars
  std::string       lineStream;
  std::stringstream objStream(dac::Strings::fromFile(filePath));

  // ----------------------------------------------------------------- //
  // -------------------------------------------------- / AUX VARS --- //


  // * --- BEGIN PARSE --- * //

  while (std::getline(objStream, lineStream)) {

    // -*Split current line in tokens
    std::vector<std::string> L = dac::Strings::split(lineStream, " ");

    // -* Avoid certain tokens
    if ((L.at(0) == "#") || (L.at(0) == "s")) { continue; }

    // --- 1. VERTICES ---------------------------------------------------- //
    if (L.at(0) == "v") { out.vertices.push_back(Math::glmVec3FromStr(L)); }
    // -------------------------------------------------------------------- //

    // --- 2. NORMALS ----------------------------------------------------- //
    if (L.at(0) == "vn") { normsTemp.push_back(Math::glmVec3FromStr(L)); }
    // -------------------------------------------------------------------- //

    // --- 3. UVs --------------------------------------------------------- //
    if (L.at(0) == "vt") { uvsTemp.push_back(Math::glmVec2FromStr(L)); }
    // -------------------------------------------------------------------- //

    // --- 4. FACES ------------------------------------------------------- //
    if (L.at(0) == "f") {

      bool thereIsUVs   = uvsTemp.size() > 0;
      bool thereIsNorms = normsTemp.size() > 0;

      // ReSize to allow the use of [] operator
      if (thereIsNorms)
        std::call_once(normsFlag,
                       [&]() { out.normals.resize(out.vertices.size()); });
      if (thereIsUVs)
        std::call_once(uvsFlag, [&]() { out.uvs.resize(out.vertices.size()); });

      // Process vertex indices and normals indices
      for (auto i = 1u; i < L.size(); ++i) {

        // Indices
        // Don't forget to substract 1 to number gived by FACE
        // i.e. "FACE.at(1) - 1u", not "FACE.at(1-1u)"
        std::vector<std::string> FACE;
        unsigned int             vIdx;
        unsigned int             nIdx;
        unsigned int             uvIdx;

        if (thereIsUVs && thereIsNorms) {
          FACE  = dac::Strings::split(L.at(i), "/");
          uvIdx = std::stoul(FACE.at(1)) - 1u;
          nIdx  = std::stoul(FACE.at(2)) - 1u;
        } else if (!thereIsUVs && thereIsNorms) {
          FACE = dac::Strings::split(L.at(i), "//");
          nIdx = std::stoul(FACE.at(1)) - 1u;
        } else if (thereIsUVs && !thereIsNorms) {
          FACE  = dac::Strings::split(L.at(i), "/");
          uvIdx = std::stoul(FACE.at(1)) - 1u;
        } else {
          dErr("[WARN] Undefined Normals and UVs @ {}", filePath);
        }
        vIdx = std::stoul(FACE.at(0)) - 1u; // allways at idx 0

        // Process if isn't cached:
        // 1. Cache the vertex Idx to not re-store N times the same normal/uv
        // 2. We use indices to draw, that indices affects to all active
        // vertex attibutes, so using the vertices indices as reference,
        // just place the normal in the corresponding vertex position

        if (thereIsNorms && !normsCache.count(vIdx) < 1) {
          normsCache.insert({vIdx, true});
          out.normals[vIdx] = normsTemp.at(nIdx);
        }

        if (thereIsUVs && !uvsCache.count(vIdx) < 1) {
          uvsCache.insert({vIdx, true});
          out.uvs[vIdx] = uvsTemp.at(uvIdx);
        }

        // Store vertex index always
        out.indices.push_back(vIdx);
      }
    }
    // -------------------------------------------------------------------- //
  }

  // * --- END PARSE --- * //

  return out;
}

} // namespace BRAVE
