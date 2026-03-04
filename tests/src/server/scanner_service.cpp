#include "scanner/v1/scanner.pb.h"
#include <gmock/gmock.h>
#include <grpcpp/client_context.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>
#include <gtest/gtest.h>

#include <Reactors/ScanResponseReactor.hpp>
#include <Service/ScannerService.hpp>
#include <Sane.hpp>
#include <SaneMock.hpp>
#include <memory>

using namespace sane_in_the_membrane::service;
using namespace sane_in_the_membrane::sane;
using namespace sane_in_the_membrane::mock;

class CScannerServiceHelper {
  public:
    GetScannersResponse& get_scanners_response() {
        grpc::CallbackServerContext context{};
        m_service_impl.GetScanners(&context, &m_get_scanner_request, &m_get_scanner_response);

        return m_get_scanner_response;
    }

    RefreshScannersResponse& refresh_devices() {
        grpc::CallbackServerContext context{};
        m_service_impl.RefreshScanners(&context, &m_refresh_scanners_request, &m_refresh_scanners_response);

        return m_refresh_scanners_response;
    }

    grpc::ServerWriteReactor<ScanResponse>* scan(const std::string& scanner_name) {
        grpc::CallbackServerContext context{};
        m_scan_request.set_scanner_name(scanner_name);

        return m_service_impl.Scan(&context, &m_scan_request);
    }

    CScannerService         m_service{};
    CScannerServiceImpl     m_service_impl{m_service};

    GetScannersRequest      m_get_scanner_request{};
    GetScannersResponse     m_get_scanner_response{};

    RefreshScannersRequest  m_refresh_scanners_request{};
    RefreshScannersResponse m_refresh_scanners_response{};

    ScanRequest             m_scan_request{};
};

class CMockGrpcServer : public ::testing::Test {
  protected:
    CMockGrpcServer() {}

    void SetUp() override {
        grpc::ServerBuilder builder;
        builder.AddListeningPort("[::]:0", grpc::InsecureServerCredentials(), &m_port);
        builder.RegisterService(&m_service_helper.m_service_impl);

        m_server = builder.BuildAndStart();
        m_server_address << "localhost:" << m_port;
    }

    void TearDown() override {
        m_server->Shutdown();
    }

    void ResetStub() {
        grpc::ChannelArguments         args;
        std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(m_server_address.str(), grpc::InsecureChannelCredentials(), args);
        m_stub                                 = std::make_unique<scanner::v1::ScannerService::Stub>(channel);
    }

    std::unique_ptr<scanner::v1::ScannerService::Stub> m_stub{};
    std::unique_ptr<grpc::Server>                      m_server{};
    std::ostringstream                                 m_server_address{};
    int                                                m_port{};
    CScannerServiceHelper                              m_service_helper{};
};

TEST(Server, EmptyScanners) {
    CScannerServiceHelper service{};
    auto                  response = service.get_scanners_response();
    EXPECT_EQ(response.scanners_size(), 0);
}

TEST(Server, GetScanners) {
    CSaneState::instance().generate_mock_device();

    CScannerServiceHelper service{};
    auto                  response = service.get_scanners_response();

    EXPECT_GT(response.scanners_size(), 0);
    EXPECT_EQ(CSaneState::instance().m_devices.front()->name, response.scanners().at(0).name());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->vendor, response.scanners().at(0).vendor());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->model, response.scanners().at(0).model());
    EXPECT_EQ(CSaneState::instance().m_devices.front()->type, response.scanners().at(0).type());
}

TEST(Server, SaneUninitialized) {
    CScannerServiceHelper service{};
    service.m_service.m_sane.exit();
    ASSERT_DEATH(service.get_scanners_response(), "");
}

TEST(Server, RefreshDevices) {
    CScannerServiceHelper service{};
    CSaneState::instance().generate_mock_device();
    service.refresh_devices();

    EXPECT_EQ(service.m_service.m_devices.shared_access()->size(), 1);
}

TEST(Server, FindDeviceNull) {
    CScannerServiceHelper service{};
    CSaneState::instance().generate_mock_device();
    service.refresh_devices();

    auto found = service.m_service.find_device("nonexistent_device");
    EXPECT_EQ(found, nullptr);
}

TEST(Server, FindDevice) {
    CScannerServiceHelper service{};
    CSaneState::instance().generate_mock_device();
    service.refresh_devices();

    auto device_name = CSaneState::instance().m_devices.front()->name;
    auto device      = service.m_service.find_device(device_name);
    EXPECT_NE(device.get(), nullptr);
    EXPECT_EQ(device->get_name(), device_name);
}

TEST(Server, ScanDeviceNotFound) {
    CScannerServiceHelper mock_service{};
    auto*                 reactor = mock_service.scan("nonexistent_scanner");

    EXPECT_NE(reactor, nullptr);
}

TEST_F(CMockGrpcServer, Scan) {
    ResetStub();

    grpc::ClientContext context{};
    ScanRequest         request{};
    ScanResponse        response{};

    CSaneState::instance().generate_mock_device();
    m_service_helper.refresh_devices();

    auto device_name = CSaneState::instance().m_devices.front()->name;
    auto device      = m_service_helper.m_service.find_device(device_name);

    request.set_scanner_name(device_name);
    auto reader = m_stub->Scan(&context, request);

    // WARN: First response never contains any data
    // WARN: Currently all test responses return 1024 bytes of data that contain all 1
    bool first_frame = true;
    while (reader->Read(&response)) {
        EXPECT_EQ(response.scanner_name(), device_name);
        if (first_frame) {
            first_frame = false;
        } else {
            EXPECT_EQ(response.data().raw_bytes().size(), 1024);
            EXPECT_EQ(response.data().raw_bytes(), std::string(1024, '\1'));
        }
    }
}
