#include "ScannerService.hpp"
#include "Sane.hpp"
#include "scanner/v1/scanner.pb.h"
#include <chrono>
#include <iostream>
#include <memory>

using namespace scanner::v1;
using namespace service;

bool CScannerServiceImpl::should_refetch_devices() const {
    return std::chrono::system_clock::now() > (m_last_device_fetch + std::chrono::seconds(30));
}

grpc::ServerUnaryReactor* CScannerServiceImpl::GetScanners(grpc::CallbackServerContext* context, const GetScannersRequest* request, GetScannersResponse* response) {

    std::cout << "Getting scanners:\n";
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        if (should_refetch_devices()) {
            std::cout << "Refetching\n";
            m_devices           = sane::g_sane->get_devices();
            m_last_device_fetch = std::chrono::system_clock::now();
        }

        std::cout << "Scan me\n";
        for (const auto& device : m_devices) {
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
