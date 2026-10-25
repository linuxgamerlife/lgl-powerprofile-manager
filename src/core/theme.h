#pragma once

#include <string>
#include <vector>

namespace ppp {

// Local files imported by a gtk.css copy. Comments and remote or absolute
// imports are ignored, because ThemeWatcher only watches files it can reach.
std::vector<std::string> local_css_imports(const std::string& css);

// Relative-luminance test for an sRGB colour, used by the theme helpers.
bool is_dark_rgb(double red, double green, double blue);

// Makes a font family safe to place inside a quoted CSS string.
std::string escape_font_family(const std::string& family);

// The monospace rule applied above the application stylesheet. It must list
// every class whose CSS uses the mono font stack so the desktop monospace font
// reaches all of them.
std::string font_stylesheet(const std::string& family);

} // namespace ppp
