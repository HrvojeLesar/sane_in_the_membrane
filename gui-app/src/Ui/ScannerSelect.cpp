#include "ScannerSelect.hpp"
#include "../Service/DeviceList.hpp"
#include "SynchronizedAccess.hpp"
#include <qvariant.h>

#include "../Utils/Globals.hpp"

using namespace sane_in_the_membrane;

ui::CScannerSelect::CScannerSelect(QWidget* parent) : QComboBox() {
    QObject::connect(&utils::Globals::get_instance().proxies()->m_device_list_proxy, &utils::proxy::CDeviceListProxy::sig_scanners_changed, this,
                     &CScannerSelect::sl_scanners_changed);
}

void ui::CScannerSelect::sl_scanners_changed(const utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
    display_items(items);
}

void ui::CScannerSelect::display_items() {
    const auto& m_scanner_items = utils::Globals::get_instance().proxies()->m_device_list_proxy.get_scanners();
    display_items(m_scanner_items);
}

void ui::CScannerSelect::display_items(const utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
    clear();
    for (const auto& scanner_item : *items) {
        addItem(scanner_item->scanner_display_name(), QVariant::fromValue(scanner_item));
    }
}

std::shared_ptr<service::CScannerItem> ui::CScannerSelect::currentData(int role) const {
    return QComboBox::currentData(role).value<std::shared_ptr<service::CScannerItem>>();
}
