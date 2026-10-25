#pragma once

#include <gtkmm.h>

#include <string>
#include <vector>

struct _GFileMonitor;
using GFileMonitor = _GFileMonitor;
struct _GSettings;
using GSettings = _GSettings;

namespace ppp {

// Initialises libadwaita and installs the application stylesheet at APPLICATION
// priority, so the user's own gtk.css still wins.
void install_application_stylesheet();

// Re-reads the user's gtk.css when it or any local file it imports changes,
// applies the desktop monospace font, tracks dark mode and the accent colour,
// and sets the .dark class used by the fixed status colours.
class ThemeWatcher {
  public:
    explicit ThemeWatcher(Gtk::Window& window);
    ~ThemeWatcher();

    ThemeWatcher(const ThemeWatcher&) = delete;
    ThemeWatcher& operator=(const ThemeWatcher&) = delete;

    sigc::signal<void()>& signal_changed();

    // The desktop monospace family last applied to the mono classes.
    [[nodiscard]] const std::string& mono_family() const;

  private:
    static void on_file_changed(GFileMonitor*, void*, void*, int, void* data);
    static int on_reload(void* data);
    static void on_style_changed(void*, void*, void* data);
    void schedule_reload();
    void apply();
    void rebuild_monitors();

    Gtk::Window& window_;
    Glib::RefPtr<Gtk::CssProvider> user_provider_;
    Glib::RefPtr<Gtk::CssProvider> monospace_provider_;
    sigc::signal<void()> changed_;
    std::string mono_family_{"Adwaita Mono"};
    std::vector<GFileMonitor*> monitors_;
    GSettings* desktop_settings_{nullptr};
    unsigned int reload_id_{0};
    unsigned long dark_handler_{0};
    unsigned long accent_handler_{0};
    unsigned long settings_handler_{0};
};

} // namespace ppp
