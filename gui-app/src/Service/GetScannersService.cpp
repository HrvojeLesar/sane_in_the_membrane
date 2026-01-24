#include "GetScannersService.hpp"

namespace service {
    CGetScannersService::CGetScannersService(scanner::v1::ScannerService::Stub& stub) : m_stub(stub) {}

    void CGetScannersService::get_scanners() {
        auto context  = std::make_shared<grpc::ClientContext>();
        auto request  = std::make_shared<scanner::v1::GetScannersRequest>();
        auto response = std::make_shared<scanner::v1::GetScannersResponse>();
        m_stub.async()->GetScanners(context.get(), request.get(), response.get(), [context, request, response, this](grpc::Status status) {
            if (!status.ok()) {
                emit this->sig_get_scanners_failed();
            } else {
                emit this->sig_get_scanners(std::move(response));
            }
        });
    }
}
