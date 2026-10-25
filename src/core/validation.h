#pragma once

#include <string>

namespace ppp {

// tuned profile names are lower-case letters, digits, hyphens, and underscores,
// 2 to 64 characters, starting with a letter. Applied before the name reaches
// pkexec, as defence in depth.
bool valid_profile_name(const std::string& name);

} // namespace ppp
