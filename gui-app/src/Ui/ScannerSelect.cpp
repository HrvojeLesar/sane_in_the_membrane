#include "ScannerSelect.hpp"
#include "../Service/DeviceList.hpp"

namespace ui {
    CScannerSelect::CScannerSelect(QWidget* parent) : QComboBox() {
        QObject::connect(service::g_device_list.get(), &service::CDeviceList::sig_scanners_changed, this, &CScannerSelect::sl_scanners_changed);
        setGeometry(50, 50, 100, 100);
    }

    void CScannerSelect::sl_scanners_changed() {
        display_items();
    }

    void CScannerSelect::display_items() {
        clear();
        const auto& m_scanner_items = service::g_device_list->get_scanners();

        for (const auto& scanner_item : m_scanner_items) {
            addItem(scanner_item->scanner_display_name(), *scanner_item.get());
        }
    }
}
