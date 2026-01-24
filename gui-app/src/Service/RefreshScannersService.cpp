#include "RefreshScannersService.hpp"

namespace service {
    CRefreshScannersService::CRefreshScannersService(scanner::v1::ScannerService::Stub& stub) : m_stub(stub) {}

    CRefreshScannersService::~CRefreshScannersService() {
        std::cout << "Destroying refresh scanners service\n";
    }

    void CRefreshScannersService::refresh_scanners() {
        auto context  = std::make_shared<grpc::ClientContext>();
        auto request  = std::make_shared<scanner::v1::RefreshScannersRequest>();
        auto response = std::make_shared<scanner::v1::RefreshScannersResponse>();
        std::cout << "Trying to refresh scanners\n";
        m_stub.async()->RefreshScanners(context.get(), request.get(), response.get(), [context, request, response, this](grpc::Status status) {
            if (!status.ok()) {
                std::cout << "Failed to refresh scanners\n";
                emit this->sig_refresh_scanners_failed();
            } else {
                std::cout << "Refreshed scanners\n";
                emit this->sig_refresh_scanners(response);
            }
        });
    }
}
