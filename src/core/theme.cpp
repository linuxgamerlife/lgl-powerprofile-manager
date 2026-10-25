#include "core/theme.h"

#include <regex>

namespace ppp {
namespace {

std::string without_comments(const std::string& css) {
    std::string result;
    result.reserve(css.size());
    std::string::size_type position = 0;
    while (position < css.size()) {
        const auto start = css.find("/*", position);
        if (start == std::string::npos) {
            result.append(css, position, std::string::npos);
            break;
        }
        result.append(css, position, start - position);
        const auto end = css.find("*/", start + 2);
        if (end == std::string::npos) {
            break;
        }
        position = end + 2;
    }
    return result;
}

bool remote_reference(const std::string& path) {
    return path.find(':') != std::string::npos || path.rfind("//", 0) == 0 ||
           (!path.empty() && path.front() == '/');
}

} // namespace

std::vector<std::string> local_css_imports(const std::string& css) {
    std::vector<std::string> paths;
    static const std::regex pattern(R"css(@import\s+(?:url\()?['"]([^'":)]+)['"]\)?)css");
    const std::string text = without_comments(css);
    for (auto iterator = std::sregex_iterator(text.begin(), text.end(), pattern);
         iterator != std::sregex_iterator(); ++iterator) {
        const std::string path = (*iterator)[1].str();
        if (!path.empty() && !remote_reference(path)) {
            paths.push_back(path);
        }
    }
    return paths;
}

bool is_dark_rgb(const double red, const double green, const double blue) {
    const double luminance = (0.2126 * red) + (0.7152 * green) + (0.0722 * blue);
    return luminance < 0.5;
}

std::string escape_font_family(const std::string& family) {
    std::string escaped;
    escaped.reserve(family.size());
    for (const unsigned char character : family) {
        if (character == '\\' || character == '"') {
            escaped.push_back('\\');
            escaped.push_back(static_cast<char>(character));
        } else if (character >= 0x20U && character != 0x7fU) {
            escaped.push_back(static_cast<char>(character));
        }
    }
    return escaped;
}

std::string font_stylesheet(const std::string& family) {
    std::string families;
    if (!family.empty()) {
        families = "\"" + escape_font_family(family) + "\", ";
    }
    families += "\"Adwaita Mono\", \"JetBrains Mono\", monospace";
    return ".mono, .wordmark, .section-label, .pp-chip, .profile-name, .reference-label, "
           ".code-block { font-family: " +
           families + "; }";
}

} // namespace ppp
