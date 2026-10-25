#include "core/profile_data.h"

#include <glib.h>

#include <set>
#include <string>

using namespace ppp;

namespace {

void builtinProfileSetIsComplete() {
    const auto& profiles = builtin_profiles();
    g_assert_cmpuint(profiles.size(), ==, 15);

    std::set<std::string> names;
    for (const auto& profile : profiles) {
        g_assert_false(profile.name.empty());
        g_assert_false(profile.label.empty());
        g_assert_false(profile.description.empty());
        g_assert_false(profile.best_for.empty());
        g_assert_false(profile.avoid_when.empty());
        g_assert_true(names.insert(profile.name).second);
    }
    g_assert_cmpstr(profiles.front().name.c_str(), ==, "balanced");
    g_assert_cmpstr(profiles.back().name.c_str(), ==, "aws");
}

void findsKnownAndCustomProfiles() {
    const auto powersave = find_profile("powersave");
    g_assert_cmpstr(powersave.name.c_str(), ==, "powersave");
    g_assert_cmpstr(powersave.label.c_str(), ==, "Max Battery");

    const auto custom = find_profile("custom-profile");
    g_assert_cmpstr(custom.name.c_str(), ==, "custom-profile");
    g_assert_true(custom.label.empty());
    g_assert_true(custom.description.empty());
}

void ordersProfilesForDisplay() {
    const auto ordered = order_profiles({"aws", "custom-one", "desktop", "balanced", "custom-one"});
    g_assert_cmpuint(ordered.size(), ==, 4);
    g_assert_cmpstr(ordered[0].c_str(), ==, "balanced");
    g_assert_cmpstr(ordered[1].c_str(), ==, "desktop");
    g_assert_cmpstr(ordered[2].c_str(), ==, "aws");
    g_assert_cmpstr(ordered[3].c_str(), ==, "custom-one");
}

void ordersUnknownProfilesAfterKnownOnes() {
    const auto ordered = order_profiles({"zzz-custom", "powersave", "aaa-custom"});
    g_assert_cmpuint(ordered.size(), ==, 3);
    g_assert_cmpstr(ordered[0].c_str(), ==, "powersave");
    g_assert_cmpstr(ordered[1].c_str(), ==, "zzz-custom");
    g_assert_cmpstr(ordered[2].c_str(), ==, "aaa-custom");
}

} // namespace

int main(int argc, char* argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/profile-data/complete-set", builtinProfileSetIsComplete);
    g_test_add_func("/profile-data/finds-known-and-custom", findsKnownAndCustomProfiles);
    g_test_add_func("/profile-data/orders-for-display", ordersProfilesForDisplay);
    g_test_add_func("/profile-data/orders-unknown-last", ordersUnknownProfilesAfterKnownOnes);
    return g_test_run();
}
