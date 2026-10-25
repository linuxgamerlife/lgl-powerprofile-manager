#pragma once

#include <gtkmm/textview.h>
#include <gtkmm/window.h>

#include <string>

namespace ppp {

// Shows the embedded HELP.md. A small Markdown renderer maps headings, bullets,
// **bold** and `code` onto text tags.
class HelpWindow final : public Gtk::Window {
  public:
    explicit HelpWindow(Gtk::Window& parent);

    // Applies the desktop monospace family to headings and code.
    void set_mono_family(const std::string& family);

  private:
    Gtk::TextView view_;
    Glib::RefPtr<Gtk::TextBuffer> buffer_;
};

} // namespace ppp
