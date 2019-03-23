#pragma once

#include <string>

namespace brave {

class Files {
public:
  static bool ok(const std::string &path, bool log = false);
};

} // namespace brave
