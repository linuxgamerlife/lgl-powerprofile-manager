#pragma once

#include <string>
#include <vector>

namespace ppp {

// Reads the profile name from `tuned-adm active` output:
//   Current active profile: balanced
// Returns an empty string when no active profile is reported.
std::string parse_active_profile(const std::string& output);

// Reads the profile names from `tuned-adm list` output. The command prints an
// "Available profiles:" heading followed by "- name" lines and a final
// "Current active profile:" line.
std::vector<std::string> parse_profile_list(const std::string& output);

} // namespace ppp
