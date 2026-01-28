#include "DeviceList.hpp"
#include "GetScannersService.hpp"
#include <memory>

namespace service {
    CScannerItem::CScannerItem(QString&& scanner_name, QString&& scanner_display_name) : m_scanner_name(scanner_name), m_scanner_display_name(scanner_display_name) {}
    CScannerItem::CScannerItem(const CScannerItem& other) : m_scanner_name(other.m_scanner_name), m_scanner_display_name(other.m_scanner_display_name) {}
    CScannerItem::CScannerItem(const CScannerItem&& other) : m_scanner_name(std::move(other.m_scanner_name)), m_scanner_display_name(std::move(other.m_scanner_display_name)) {}
    CScannerItem::CScannerItem(const scanner::v1::ScannerInfo& scanner_info) :
        m_scanner_name(QString::fromStdString(std::format("{}", scanner_info.name()))),
        m_scanner_display_name(QString::fromStdString(std::format("{} {}", scanner_info.vendor(), scanner_info.model()))) {}
    CScannerItem::~CScannerItem() {}

    CDeviceList::CDeviceList() {
        QObject::connect(g_get_scanner_service.get(), &CGetScannersService::sig_get_scanners_failed, this, &CDeviceList::sl_get_scanners_failed);
        QObject::connect(g_get_scanner_service.get(), &CGetScannersService::sig_get_scanners, this, &CDeviceList::sl_get_scanners);
    }
    CDeviceList::~CDeviceList() {}

    void CDeviceList::add_scanner_item(const CScannerItem scanner_item) {
        {
            auto scanners = m_scanners.access();
            scanners->push_back(std::make_shared<CScannerItem>(std::move(scanner_item)));
        }
        auto shared_access = m_scanners.shared_access();
        emit sig_scanners_changed(shared_access);
    }

    void CDeviceList::set_scanner_items(const std::vector<std::shared_ptr<CScannerItem>>& items) {
        {
            clear_internal();
            auto scanners = m_scanners.access();
            for (const auto& scanner_item : items) {
                scanners->emplace_back(scanner_item);
            }
        }

        auto shared_access = m_scanners.shared_access();
        emit sig_scanners_changed(shared_access);
    }
    const SharedAccessGuard<std::vector<std::shared_ptr<CScannerItem>>> CDeviceList::get_scanners() const {
        return m_scanners.shared_access();
    }

    void CDeviceList::clear() {
        clear_internal();
        auto shared_access = m_scanners.shared_access();
        emit sig_scanners_changed(shared_access);
    }

    void CDeviceList::sl_get_scanners_failed() {
        std::cout << "Device List - Failed to get scanners\n";
    }

    void CDeviceList::sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response) {
        std::vector<std::shared_ptr<CScannerItem>> scanners;
        for (const auto& scanner : response->scanners()) {
            scanners.push_back(std::make_shared<CScannerItem>(scanner));
        }

        set_scanner_items(scanners);
    }

    void CDeviceList::clear_internal() {
        auto scanners = m_scanners.access();
        scanners->clear();
    }
}
