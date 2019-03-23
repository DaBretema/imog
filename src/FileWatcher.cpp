#include "FileWatcher.hpp"
#include "Async.hpp"
#include "Logger.hpp"

namespace brave {

// ====================================================================== //
// ====================================================================== //
// w/ default ctor the filewatcher is NOT launced directly
// is needed to call "launch()" after define "path" and "callback"
// ====================================================================== //

FileWatcher::FileWatcher()
    : m_verbose(false), m_launched(false), m_threadLive(true) {}

// ====================================================================== //
// ====================================================================== //
// w/ param ctor the filewatcher is launced directly
// w/o call "launch()"
// ====================================================================== //

FileWatcher::FileWatcher(const std::string&   filepath,
                         const filewatcherFn& callback,
                         bool                 verbose)
    : m_verbose(verbose),
      m_launched(false),
      m_threadLive(true),
      m_path(""),
      m_callback(nullptr) {
  launch(filepath, callback);
}

// ====================================================================== //
// ====================================================================== //
// Destroy the object and kill thread dependent of this object
// ====================================================================== //

FileWatcher::~FileWatcher() {
  m_threadLive = false;
  if (m_verbose) { LOGD("FileWatcher destroyed @ '{}'", m_path); }
}

// ====================================================================== //
// ====================================================================== //
// Verify if a file was modified since last time that was loaded
// ====================================================================== //

bool FileWatcher::fileHasBeenModified() {
  std::fstream f(m_path);

  // Get stream as string
  std::stringstream m_auxContent;
  m_auxContent << f.rdbuf();
  std::string m_auxContentStr = m_auxContent.str();

  bool modified = m_refContentStr != m_auxContentStr;
  // Avoid copy if not needed
  if (modified) { m_refContentStr = m_auxContentStr; }
  return modified;
}

// ====================================================================== //
// ====================================================================== //
// If file has been modified trigger the stored callback function
// ====================================================================== //

void FileWatcher::update() {
  if (fileHasBeenModified()) {
    std::fstream fs(m_path);
    m_callback(fs);
  }
}

// ====================================================================== //
// ====================================================================== //
// Launch the thread to active filewatcher
// ====================================================================== //

bool FileWatcher::launch(const std::string& path, const filewatcherFn& fn) {

  if (m_launched) {
    LOGE("FileWatcher already active @ '{}'", m_path);
    return true;
  }
  if (path == "") {
    LOGE("Undefined path @ FileWatcher");
    return false;
  }
  if (fn == nullptr) {
    LOGE("Empty callback @ '{}' FileWatcher", m_path);
    return false;
  }

  m_path       = path;
  m_callback   = fn;
  m_threadLive = true;

  Async::periodic([]() { return 0.5f; }, &m_threadLive, [&]() { update(); });
  m_launched = true;

  if (m_verbose) { LOGD("FileWatcher created @ '{}'", m_path); }
  return true;
}

// ====================================================================== //
// ====================================================================== //
// Setter for verbose
// ====================================================================== //

void FileWatcher::verbose(bool newState) { m_verbose = newState; }

} // namespace brave
