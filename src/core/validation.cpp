#include "core/validation.h"

namespace ppp {

bool valid_profile_name(const std::string& name) {
    if (name.size() < 2 || name.size() > 64) {
        return false;
    }
    const auto is_lower = [](const char character) { return character >= 'a' && character <= 'z'; };
    const auto is_digit = [](const char character) { return character >= '0' && character <= '9'; };
    if (!is_lower(name.front())) {
        return false;
    }
    for (const char character : name) {
        if (!is_lower(character) && !is_digit(character) && character != '-' && character != '_') {
            return false;
        }
    }
    return true;
}

} // namespace ppp
