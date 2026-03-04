#include "ScannerService.hpp"
#include "Sane.hpp"
#include "../Reactors/ScanResponseReactor.hpp"
#include "SaneDevice.hpp"
#include <memory>
#include <GLogger.hpp>

using namespace scanner::v1;
using namespace sane_in_the_membrane;
using namespace sane_in_the_membrane::service;

bool CScannerService::should_refresh_devices() const {
    return std::chrono::system_clock::now() > (*m_last_device_fetch.shared_access() + std::chrono::seconds(30));
}

std::shared_ptr<sane::CSaneDevice> CScannerService::find_device(const std::string& name) const {
    for (const auto& device : *m_devices.access()) {
        if (!device.expired()) {
            auto d = device.lock();
            if (d->get_name() == name) {
                return d;
            }
        }
    }

    return nullptr;
}

void CScannerService::refresh_devices() {
    if (should_refresh_devices()) {
        log::debug("Refreshing scanners");
        auto devices           = m_devices.access();
        auto last_device_fetch = m_last_device_fetch.access();

        *devices           = m_sane.get_devices();
        *last_device_fetch = std::chrono::system_clock::now();
    } else {
        log::debug("Skipped refreshing");
    }
}

CScannerServiceImpl::CScannerServiceImpl(CScannerService& service) : m_service(service) {}

grpc::ServerUnaryReactor* CScannerServiceImpl::GetScanners(grpc::CallbackServerContext* context, const GetScannersRequest* request, GetScannersResponse* response) {

    log::debug("Getting scanners");
    {
        if (m_service.m_devices.shared_access()->empty()) {
            m_service.refresh_devices();
        }

        for (const auto& device : *m_service.m_devices.access()) {
            if (!device.expired()) {
                auto d          = device.lock();
                auto raw_device = d->get_raw_device();

                auto scanner_info = response->add_scanners();
                scanner_info->set_name(d->get_name());

                if (raw_device != nullptr) {
                    scanner_info->set_model(raw_device->model);
                    scanner_info->set_vendor(raw_device->vendor);
                    scanner_info->set_type(raw_device->type);
                }
            }
        }
    }

    auto* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}

grpc::ServerWriteReactor<ScanResponse>* CScannerServiceImpl::Scan(grpc::CallbackServerContext* context, const ScanRequest* request) {
    log::debug("Scanning");
    std::shared_ptr<sane::CSaneDevice> device{};
    {
        auto& scanner_name = request->scanner_name();
        device             = m_service.find_device(scanner_name);
    }

    if (!device)
        return new reactor::CEmptyScanResponseReactor();

    return new reactor::CScanResponseReactor(device);
}

grpc::ServerUnaryReactor* CScannerServiceImpl::RefreshScanners(grpc::CallbackServerContext* context, const RefreshScannersRequest* request, RefreshScannersResponse* response) {
    m_service.refresh_devices();

    auto* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}
