#pragma once

#include <fstream>
#include <sstream>
#include <functional>

namespace DAC {

class FileWatcher {

  using filewatcherFn = std::function<void(std::fstream)>;

private:
  std::string       m_refContentStr;
  std::string       m_auxContentStr;
  std::stringstream m_auxContent;

  bool m_threadLive;
  bool m_allowPrintInfo;

  std::string   m_path;
  filewatcherFn m_callback;

  bool fileHasBeenModified();


public:
  FileWatcher();

  FileWatcher(const std::string& filepath, const filewatcherFn& callback,
              bool allowPrintInfo = true);

  ~FileWatcher();

  void update();

  void launchWatcher();
  void setPath(const std::string& filepath);
  void setCallback(const filewatcherFn& callback);
};

} // namespace DAC
