#include "ScannerSelect.hpp"
#include "../Service/DeviceList.hpp"
#include "SynchronizedAccess.hpp"

namespace ui {
    CScannerSelect::CScannerSelect(QWidget* parent) : QComboBox() {
        QObject::connect(service::g_device_list.get(), &service::CDeviceList::sig_scanners_changed, this, &CScannerSelect::sl_scanners_changed);
        setGeometry(50, 50, 100, 100);
    }

    void CScannerSelect::sl_scanners_changed(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
        display_items(items);
    }

    void CScannerSelect::display_items() {
        const auto& m_scanner_items = service::g_device_list->get_scanners();
        display_items(m_scanner_items);
    }

    void CScannerSelect::display_items(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
        clear();
        for (const auto& scanner_item : *items) {
            addItem(scanner_item->scanner_display_name(), *scanner_item.get());
        }
    }
}
