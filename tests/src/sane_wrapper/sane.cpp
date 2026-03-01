#include <gtest/gtest.h>

#include <Sane.hpp>
#include <sane/sane.h>

using namespace sane_in_the_membrane::sane;

TEST(Sane, SaneInit) {
    CSane       sane{};
    CSaneStatus status{SANE_STATUS_GOOD};
    EXPECT_EQ(SANE_STATUS_GOOD, sane.init());
    EXPECT_EQ(true, sane.is_ok());
}
