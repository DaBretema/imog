#include "Files.hpp"

#include <fstream>
#include <sys/stat.h>

#include "Logger.hpp"


namespace brave {

bool Files::ok(const std::string& path, bool log) {
  bool ok = std::fstream(path).good();
  if (log && !ok) LOGD("Problems with file {}", path);
  return ok;
}

bool Files::pathExists(const std::string& path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

std::string Files::pathToWin(const std::string& path) {
  auto auxStr = path;
  std::replace(auxStr.begin(), auxStr.end(), '/', '\\');
  return auxStr;
}

} // namespace brave
