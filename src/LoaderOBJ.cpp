
#include <mutex>
#include <sstream>
#include <unordered_map>

#include <Dac/Strings.hpp>

#include "../incl/LoaderOBJ.hpp"


namespace BRAVE {


/// Wrap the process to load a 3D obj fromo OBJ file
RenderData loadOBJ(const std::string& filePath) {
  RenderData out;

  // Temp vars
  std::once_flag                         normsFlag;
  std::vector<glm::vec3>                 normsTemp;
  std::unordered_map<unsigned int, bool> normsCache;

  // Str vars
  std::string       lineStream;
  std::stringstream objStream(DAC::Strings::fromFile(filePath));

  // Parse process
  while (std::getline(objStream, lineStream)) {

    // Split current line in tokens
    std::vector<std::string> LINE = DAC::Strings::split(lineStream, " ");

    // Avoid coment lines and short lines
    if (LINE.size() > 3u && !(LINE.at(0) == "#")) {

      // VERTICES
      if (LINE.at(0) == "v") {
        glm::vec3 v(std::stof(LINE.at(1)),
                    std::stof(LINE.at(2)),
                    std::stof(LINE.at(3)));
        out.vertices.push_back(v);
      }


      // NORMALS
      if (LINE.at(0) == "vn") {
        glm::vec3 n(std::stof(LINE.at(1)),
                    std::stof(LINE.at(2)),
                    std::stof(LINE.at(3)));
        normsTemp.push_back(n);
      }


      // FACES
      if (LINE.at(0) == "f") {
        // ReSize to allow the use of [] operator (Do it only once using flag)
        std::call_once(normsFlag,
                       [&]() { out.normals.resize(out.vertices.size()); });
        // Process vertex indices and normals indices
        for (auto i = 1u; i < LINE.size(); ++i) {
          std::vector<std::string> FACE = DAC::Strings::split(LINE.at(i), "//");

          // Indices :: The -1 operation is crucial
          unsigned int vIdx = std::stoul(FACE.at(0)) - 1u;
          unsigned int nIdx = std::stoul(FACE.at(1)) - 1u;

          // Only process Normals if isn't cached
          if (normsCache.count(vIdx) < 1) {

            // Cache the vertex Idx to not re-store N times the same normal
            normsCache.insert({ vIdx, true });

            // We use indices to draw, that indices affects to all active
            // vertex attibutes, so using the vertices indices as reference,
            // and avoiding to repeat a lot of info, just place the normal
            // in the corresponding vertex position
            out.normals[vIdx] = normsTemp.at(nIdx);
          }


          // Store vertex index always
          out.indices.push_back(vIdx);
        }
      }
    }
  }

  // Return a RenderData struct
  return out;
}

} // namespace BRAVE
