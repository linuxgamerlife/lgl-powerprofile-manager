#include "core/theme.h"

#include <glib.h>

#include <string>

using namespace ppp;

namespace {

void followsLocalImports() {
    const auto imports = local_css_imports("/* @import url('ignored.css'); */\n"
                                           "@import url('palette.css');\n"
                                           "@import \"sub/extra.css\";\n"
                                           "@import url(\"https://example.com/remote.css\");\n"
                                           "@import url('/absolute.css');\n");
    g_assert_cmpuint(imports.size(), ==, 2);
    g_assert_cmpstr(imports[0].c_str(), ==, "palette.css");
    g_assert_cmpstr(imports[1].c_str(), ==, "sub/extra.css");
}

void detectsDarkBackgrounds() {
    g_assert_true(is_dark_rgb(0.1, 0.1, 0.1));
    g_assert_false(is_dark_rgb(0.9, 0.9, 0.9));
    g_assert_true(is_dark_rgb(0.0, 0.0, 0.0));
    g_assert_false(is_dark_rgb(1.0, 1.0, 1.0));
}

void escapesFontFamilies() {
    const auto plain = escape_font_family("Adwaita Mono");
    const auto quoted = escape_font_family("Fira \"Code\"");
    const auto backslash = escape_font_family("Back\\slash");
    const auto newline = escape_font_family("Line\nBreak");
    g_assert_cmpstr(plain.c_str(), ==, "Adwaita Mono");
    g_assert_cmpstr(quoted.c_str(), ==, "Fira \\\"Code\\\"");
    g_assert_cmpstr(backslash.c_str(), ==, "Back\\\\slash");
    g_assert_cmpstr(newline.c_str(), ==, "LineBreak");
}

void buildsTheMonospaceStylesheet() {
    const auto rule = font_stylesheet("Fira \"Code\"");
    g_assert_true(rule.find("Fira \\\"Code\\\"") != std::string::npos);
    g_assert_true(rule.find("\"Adwaita Mono\"") != std::string::npos);
    for (const char* name : {".mono", ".wordmark", ".section-label", ".pp-chip", ".profile-name",
                             ".reference-label", ".code-block"}) {
        g_assert_true(rule.find(name) != std::string::npos);
    }

    const auto fallback = font_stylesheet("");
    g_assert_true(fallback.find("\"\"") == std::string::npos);
    g_assert_true(fallback.find("\"Adwaita Mono\"") != std::string::npos);
}

} // namespace

int main(int argc, char* argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/theme/follows-local-imports", followsLocalImports);
    g_test_add_func("/theme/detects-dark-backgrounds", detectsDarkBackgrounds);
    g_test_add_func("/theme/escapes-font-families", escapesFontFamilies);
    g_test_add_func("/theme/builds-monospace-stylesheet", buildsTheMonospaceStylesheet);
    return g_test_run();
}
