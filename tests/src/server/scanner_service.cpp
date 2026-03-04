#include "scanner/v1/scanner.pb.h"
#include <grpcpp/server_context.h>
#include <gtest/gtest.h>

#include <Service/ScannerService.hpp>
#include <Sane.hpp>
#include <SaneMock.hpp>

using namespace sane_in_the_membrane::service;
using namespace sane_in_the_membrane::sane;
using namespace sane_in_the_membrane::mock;

class CGetScannersServiceMock {
  public:
    GetScannersResponse& get_scanners_response() {
        service_impl.GetScanners(&context, &request, &response);

        return response;
    }

    CScannerService             service{};
    CScannerServiceImpl         service_impl{service};

    grpc::CallbackServerContext context{};
    GetScannersRequest          request{};
    GetScannersResponse         response{};
};

TEST(Server, EmptyScanners) {
    CGetScannersServiceMock service{};
    auto                    response = service.get_scanners_response();
    EXPECT_EQ(response.scanners_size(), 0);
}

TEST(Server, GetScanners) {
    CSaneState::instance().generate_mock_device();

    CGetScannersServiceMock service{};
    auto                    response = service.get_scanners_response();

    EXPECT_GT(response.scanners_size(), 0);
    EXPECT_EQ(CSaneState::instance().m_devices.front()->name, response.scanners().at(0).name());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->vendor, response.scanners().at(0).vendor());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->model, response.scanners().at(0).model());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->type, response.scanners().at(0).type());
}

TEST(Server, SaneUninitialized) {
    CGetScannersServiceMock service{};
    service.service.m_sane.exit();
    ASSERT_DEATH(service.get_scanners_response(), "");
}
