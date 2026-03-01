#include <gtest/gtest.h>

#include <Sane.hpp>
#include <sane/sane.h>
#include <SaneMock.hpp>

using namespace sane_in_the_membrane::sane;
using namespace sane_in_the_membrane::mock;

TEST(Sane, SaneInit) {
    CSane       sane{};
    CSaneStatus status{SANE_STATUS_GOOD};
    EXPECT_EQ(SANE_STATUS_GOOD, sane.init());
    EXPECT_EQ(true, sane.is_ok());
}

TEST(Sane, OpenDevice) {
    CSane sane{};
    CSaneState::instance().generate_mock_device();

    auto devices = sane.get_devices();
    ASSERT_EQ(1, devices.size());

    auto device = devices.front().lock();

    EXPECT_EQ(SANE_STATUS_GOOD, device->open());
    EXPECT_EQ(SANE_STATUS_GOOD, device->start());
    EXPECT_EQ(SANE_STATUS_GOOD, device->read());
    EXPECT_EQ(CMockHandle::MAX_READ_LENGTH, device->buffer().read_len);
}
