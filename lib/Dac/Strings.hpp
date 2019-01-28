#pragma once

#include <string>
#include <vector>

namespace DAC {

class Strings {
public:
  /// Get as std::string the content of a given file
  static std::string fromFile(const std::string& filePath);

  /// split gives a vector with splitted strings
  static std::vector<std::string> split(const std::string& str,
                                        const std::string& delimeter);
};

} // namespace DAC
