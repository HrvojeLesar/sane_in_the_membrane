#include "DeviceList.hpp"
#include "GetScannersService.hpp"
#include <memory>

namespace service {
    CScannerItem::CScannerItem(QString&& scanner_name, QString&& scanner_display_name) : QVariant(), m_scanner_name(scanner_name), m_scanner_display_name(scanner_display_name) {}
    CScannerItem::CScannerItem(const CScannerItem& other) : QVariant() {}
    CScannerItem::CScannerItem(const CScannerItem&& other) :
        QVariant(), m_scanner_name(std::move(other.m_scanner_name)), m_scanner_display_name(std::move(other.m_scanner_display_name)) {}
    CScannerItem::CScannerItem(const scanner::v1::ScannerInfo& scanner_info) :
        QVariant(), m_scanner_name(QString::fromStdString(std::format("{}", scanner_info.name()))),
        m_scanner_display_name(QString::fromStdString(std::format("{} {}", scanner_info.vendor(), scanner_info.model()))) {}
    CScannerItem::~CScannerItem() {}

    CDeviceList::CDeviceList() {
        QObject::connect(g_get_scanner_service.get(), &CGetScannersService::sig_get_scanners_failed, this, &CDeviceList::sl_get_scanners_failed);
        QObject::connect(g_get_scanner_service.get(), &CGetScannersService::sig_get_scanners, this, &CDeviceList::sl_get_scanners);
    }
    CDeviceList::~CDeviceList() {}

    void CDeviceList::add_scanner_item(const CScannerItem scanner_item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_scanners.push_back(std::make_shared<CScannerItem>(std::move(scanner_item)));
        emit sig_scanners_changed(m_scanners);
    }

    void CDeviceList::set_scanner_items(const std::vector<std::shared_ptr<CScannerItem>>& items) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& scanner_item : items) {
            m_scanners.emplace_back(scanner_item);
        }

        emit sig_scanners_changed(m_scanners);
    }
    const std::vector<std::shared_ptr<CScannerItem>>& CDeviceList::get_scanners() const {
        return m_scanners;
    }

    void CDeviceList::clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_scanners.clear();
        emit sig_scanners_changed(m_scanners);
    }

    void CDeviceList::sl_get_scanners_failed() {
        std::cout << "Failed to get scanners\n";
    }

    void CDeviceList::sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response) {
        std::vector<std::shared_ptr<CScannerItem>> scanners;
        for (const auto& scanner : response->scanners()) {
            scanners.push_back(std::make_shared<CScannerItem>(scanner));
        }

        set_scanner_items(scanners);
    }
}
