#include "core/tuned.h"

#include <glib.h>

#include <string>

using namespace ppp;

namespace {

void readsTheActiveProfile() {
    const auto balanced = parse_active_profile("Current active profile: balanced\n");
    const auto guest = parse_active_profile("Current active profile:   virtual-guest");
    g_assert_cmpstr(balanced.c_str(), ==, "balanced");
    g_assert_cmpstr(guest.c_str(), ==, "virtual-guest");
}

void ignoresUnknownAndMissingProfiles() {
    const auto empty = parse_active_profile("");
    const auto unknown = parse_active_profile("Current active profile: unknown");
    const auto dash = parse_active_profile("Current active profile: -");
    const auto not_running = parse_active_profile("tuned is not running");
    g_assert_true(empty.empty());
    g_assert_true(unknown.empty());
    g_assert_true(dash.empty());
    g_assert_true(not_running.empty());
}

void readsTheProfileList() {
    const std::string output = "Available profiles:\n"
                               "- balanced\n"
                               "- desktop\n"
                               "- latency-performance\n"
                               "\n"
                               "Current active profile: desktop\n";
    const auto profiles = parse_profile_list(output);
    g_assert_cmpuint(profiles.size(), ==, 3);
    g_assert_cmpstr(profiles[0].c_str(), ==, "balanced");
    g_assert_cmpstr(profiles[1].c_str(), ==, "desktop");
    g_assert_cmpstr(profiles[2].c_str(), ==, "latency-performance");
}

void removesDuplicateProfiles() {
    const auto profiles = parse_profile_list("- balanced\n- balanced\n- powersave\n");
    g_assert_cmpuint(profiles.size(), ==, 2);
    g_assert_cmpstr(profiles[0].c_str(), ==, "balanced");
    g_assert_cmpstr(profiles[1].c_str(), ==, "powersave");
}

void handlesEmptyOutput() {
    const auto empty = parse_profile_list("");
    const auto none = parse_profile_list("No profiles available\n");
    g_assert_true(empty.empty());
    g_assert_true(none.empty());
}

} // namespace

int main(int argc, char* argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/tuned/reads-active-profile", readsTheActiveProfile);
    g_test_add_func("/tuned/ignores-unknown-and-missing", ignoresUnknownAndMissingProfiles);
    g_test_add_func("/tuned/reads-profile-list", readsTheProfileList);
    g_test_add_func("/tuned/removes-duplicates", removesDuplicateProfiles);
    g_test_add_func("/tuned/handles-empty-output", handlesEmptyOutput);
    return g_test_run();
}
