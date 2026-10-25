#pragma once

#include <filesystem>

namespace ppp {

// Preferences stored in $XDG_CONFIG_HOME/lgl-powerprofile-manager/settings.ini.
// Every setter saves immediately, and any missing or malformed value falls back
// to its default. A second constructor takes a path for tests.
class Settings {
  public:
    Settings();
    explicit Settings(std::filesystem::path path);

    [[nodiscard]] bool system_title_bar() const;
    void set_system_title_bar(bool enabled);

  private:
    void load();
    void save() const;

    std::filesystem::path path_;
    bool system_title_bar_{false};
};

} // namespace ppp
