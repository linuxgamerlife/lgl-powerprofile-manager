#include "core/settings.h"

#include <glib.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace ppp;

namespace {

struct TemporaryConfig {
    std::string folder;
    std::string path;

    TemporaryConfig() {
        gchar* created = g_dir_make_tmp("powerprofile-settings-XXXXXX", nullptr);
        g_assert_nonnull(created);
        folder = created;
        g_free(created);
        path = folder + "/lgl-powerprofile-manager/settings.ini";
    }
    ~TemporaryConfig() {
        std::error_code error;
        std::filesystem::remove_all(folder, error);
    }
    void write(const std::string& contents) const {
        std::filesystem::create_directories(folder + "/lgl-powerprofile-manager");
        std::ofstream(path) << contents;
    }
};

void missingFileUsesDefaults() {
    TemporaryConfig config;
    const Settings settings(config.path);
    g_assert_false(settings.system_title_bar());
    g_assert_false(std::filesystem::exists(config.path));
}

void systemTitleBarPersists() {
    TemporaryConfig config;
    Settings(config.path).set_system_title_bar(true);
    g_assert_true(std::filesystem::exists(config.path));
    g_assert_true(Settings(config.path).system_title_bar());

    Settings(config.path).set_system_title_bar(false);
    g_assert_false(Settings(config.path).system_title_bar());
}

void malformedValuesUseDefaults() {
    TemporaryConfig config;
    config.write("[window]\nsystemTitleBar=maybe\n");
    g_assert_false(Settings(config.path).system_title_bar());

    config.write("not a key file");
    g_assert_false(Settings(config.path).system_title_bar());
}

void missingGroupUsesDefaults() {
    TemporaryConfig config;
    config.write("[display]\nscale=2\n");
    g_assert_false(Settings(config.path).system_title_bar());
}

} // namespace

int main(int argc, char* argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/settings/missing-file-uses-defaults", missingFileUsesDefaults);
    g_test_add_func("/settings/system-title-bar-persists", systemTitleBarPersists);
    g_test_add_func("/settings/malformed-values-use-defaults", malformedValuesUseDefaults);
    g_test_add_func("/settings/missing-group-uses-defaults", missingGroupUsesDefaults);
    return g_test_run();
}
