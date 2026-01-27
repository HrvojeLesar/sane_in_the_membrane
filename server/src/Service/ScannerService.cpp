#include "ScannerService.hpp"
#include "Sane.hpp"
#include "../Reactors/ScanResponseReactor.hpp"
#include "SaneDevice.hpp"
#include <memory>

using namespace scanner::v1;
using namespace service;

bool CScannerServiceImpl::should_refresh_devices() const {
    return std::chrono::system_clock::now() > (*m_last_device_fetch.shared_access() + std::chrono::seconds(30));
}

grpc::ServerUnaryReactor* CScannerServiceImpl::GetScanners(grpc::CallbackServerContext* context, const GetScannersRequest* request, GetScannersResponse* response) {

    std::cout << "Getting scanners:\n";
    {
        if (m_devices.shared_access()->empty()) {
            refresh_devices();
        }

        for (const auto& device : *m_devices.access()) {
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
    std::cout << "Scanning\n";
    std::shared_ptr<sane::CSaneDevice> device{};
    {

        auto& scanner_name = request->scanner_name();
        device             = find_device(scanner_name);
    }

    return new reactor::CScanResponseReactor(device);
}

std::shared_ptr<sane::CSaneDevice> CScannerServiceImpl::find_device(const std::string& name) const {
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

void CScannerServiceImpl::refresh_devices() {
    if (should_refresh_devices()) {
        std::cout << "Refreshing scanners\n";
        m_devices           = sane::g_sane->get_devices();
        m_last_device_fetch = std::chrono::system_clock::now();
    } else {
        std::cout << "Skipped refreshing\n";
    }
}

grpc::ServerUnaryReactor* CScannerServiceImpl::RefreshScanners(grpc::CallbackServerContext* context, const RefreshScannersRequest* request, RefreshScannersResponse* response) {
    std::cout << "Refreshing scanners:\n";
    refresh_devices();

    auto* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}
