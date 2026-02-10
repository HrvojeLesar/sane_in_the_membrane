#include "RefreshScannersService.hpp"
#include "../GlobalLogger.cpp"

using namespace sane_in_the_membrane::service;

CRefreshScannersService::CRefreshScannersService(std::shared_ptr<scanner::v1::ScannerService::Stub>& stub) : m_stub(stub) {}

CRefreshScannersService::~CRefreshScannersService() {
    g_logger.log(TRACE, "Destroying refresh scanners service");
}

void CRefreshScannersService::refresh_scanners() {
    auto context  = std::make_shared<grpc::ClientContext>();
    auto request  = std::make_shared<scanner::v1::RefreshScannersRequest>();
    auto response = std::make_shared<scanner::v1::RefreshScannersResponse>();
    g_logger.log(INFO, "Trying to refresh scanners");
    m_stub->async()->RefreshScanners(context.get(), request.get(), response.get(), [context, request, response, this](grpc::Status status) {
        if (!status.ok()) {
            g_logger.log(WARN, "Failed to refresh scanners");
            emit this->sig_refresh_scanners_failed();
        } else {
            g_logger.log(INFO, "Refreshed scanners");
            emit this->sig_refresh_scanners(response);
        }
    });
}
