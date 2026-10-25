#include "core/validation.h"

#include <glib.h>

#include <string>

using namespace ppp;

namespace {

void acceptsValidProfileNames() {
    g_assert_true(valid_profile_name("balanced"));
    g_assert_true(valid_profile_name("latency-performance"));
    g_assert_true(valid_profile_name("custom_1"));
    g_assert_true(valid_profile_name("ab"));
    g_assert_true(valid_profile_name("a" + std::string(63, 'b')));
}

void rejectsInvalidProfileNames() {
    g_assert_false(valid_profile_name(""));
    g_assert_false(valid_profile_name("a"));
    g_assert_false(valid_profile_name("Balanced"));
    g_assert_false(valid_profile_name("1balanced"));
    g_assert_false(valid_profile_name("bad name"));
    g_assert_false(valid_profile_name("bad.name"));
    g_assert_false(valid_profile_name("-balanced"));
    g_assert_false(valid_profile_name("a" + std::string(64, 'b')));
    g_assert_false(valid_profile_name(std::string("balanced\0extra", 14)));
}

} // namespace

int main(int argc, char* argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/validation/accepts-valid", acceptsValidProfileNames);
    g_test_add_func("/validation/rejects-invalid", rejectsInvalidProfileNames);
    return g_test_run();
}
