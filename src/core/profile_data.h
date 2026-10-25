#pragma once

#include <string>
#include <vector>

namespace ppp {

// Static metadata for the built-in tuned profiles. The order is the order used
// by the Profiles and Reference pages: desktop use first, then gaming, network,
// virtualisation, battery, and specialised profiles.
struct ProfileInfo {
    std::string name;
    std::string label;
    std::string description;
    std::string best_for;
    std::string avoid_when;
};

const std::vector<ProfileInfo>& builtin_profiles();

// Returns the entry for name, or a stub that keeps the name and leaves the
// descriptive fields empty (custom profiles installed outside tuned's set).
ProfileInfo find_profile(const std::string& name);

// Puts the profiles reported by tuned in built-in order, appending any custom
// profiles at the end. Duplicate names are kept only once.
std::vector<std::string> order_profiles(const std::vector<std::string>& available);

} // namespace ppp
