#include "Files.hpp"

#include <fstream>
#include "Logger.hpp"

namespace brave {

bool Files::ok(const std::string& path, bool log) {
  bool ok = std::fstream(path).good();
  if (log && !ok) LOGD("Problems with file {}", path);
  return ok;
}

} // namespace brave
