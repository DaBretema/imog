#include "LoaderOBJ.hpp"

#include <mutex>
#include <sstream>
#include <unordered_map>

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

  // Temp vars
  std::once_flag                         normsFlag;
  std::vector<glm::vec3>                 normsTemp;
  std::unordered_map<unsigned int, bool> normsCache;

  // Str vars
  std::string       lineStream;
  std::stringstream objStream(dac::Strings::fromFile(filePath));

  // ----------------------------------------------------------------- //
  // -------------------------------------------------- / AUX VARS --- //


  // * --- BEGIN PARSE --- * //

  while (std::getline(objStream, lineStream)) {

    // -*Split current line in tokens
    std::vector<std::string> L = dac::Strings::split(lineStream, " ");

    // -* Avoid coment lines and short lines
    if ((L.size() <= 3u) || (L.at(0) == "#")) { continue; }
    // if (!(L.size() > 3u && !(L.at(0) == "#"))) { continue; }

    // --- 1. VERTICES ---------------------------------------------------- //
    if (L.at(0) == "v") { out.vertices.push_back(Math::str3ToVec3(L)); }
    // -------------------------------------------------------------------- //

    // --- 2. NORMALS ----------------------------------------------------- //
    if (L.at(0) == "vn") { normsTemp.push_back(Math::str3ToVec3(L)); }
    // -------------------------------------------------------------------- //

    // --- 3. FACES ------------------------------------------------------- //
    if (L.at(0) == "f") {
      // ReSize to allow the use of [] operator (Do it only once using flag)
      // std::call_once(normsFlag, out.normals.resize, out.vertices.size());
      std::call_once(normsFlag,
                     [&]() { out.normals.resize(out.vertices.size()); });

      // Process vertex indices and normals indices
      for (auto i = 1u; i < L.size(); ++i) {

        // Indices :: The -1 operation is crucial
        std::vector<std::string> FACE = dac::Strings::split(L.at(i), "//");
        unsigned int             vIdx = std::stoul(FACE.at(0)) - 1u;
        unsigned int             nIdx = std::stoul(FACE.at(1)) - 1u;

        // Only process Normals if isn't cached
        if (!normsCache.count(vIdx) < 1) {

          // Cache the vertex Idx to not re-store N times the same normal
          normsCache.insert({vIdx, true});

          // We use indices to draw, that indices affects to all active
          // vertex attibutes, so using the vertices indices as reference,
          // just place the normal in the corresponding vertex position
          out.normals[vIdx] = normsTemp.at(nIdx);
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
