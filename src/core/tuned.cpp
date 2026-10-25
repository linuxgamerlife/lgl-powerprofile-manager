#include "core/tuned.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <set>

namespace ppp {
namespace {

std::string trimmed(std::string value) {
    const auto not_space = [](const unsigned char character) {
        return std::isspace(character) == 0;
    };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

} // namespace

std::string parse_active_profile(const std::string& output) {
    static const std::regex pattern(R"(Current active profile:\s*(\S+))");
    std::smatch match;
    if (!std::regex_search(output, match, pattern)) {
        return {};
    }
    const std::string name = trimmed(match[1].str());
    if (name.empty() || name == "unknown" || name == "-") {
        return {};
    }
    return name;
}

std::vector<std::string> parse_profile_list(const std::string& output) {
    static const std::regex pattern(R"(^-\s+(\S+))", std::regex_constants::multiline);
    std::vector<std::string> profiles;
    std::set<std::string> seen;
    for (auto iterator = std::sregex_iterator(output.begin(), output.end(), pattern);
         iterator != std::sregex_iterator(); ++iterator) {
        const std::string name = trimmed((*iterator)[1].str());
        if (name.empty() || !seen.insert(name).second) {
            continue;
        }
        profiles.push_back(name);
    }
    return profiles;
}

} // namespace ppp
