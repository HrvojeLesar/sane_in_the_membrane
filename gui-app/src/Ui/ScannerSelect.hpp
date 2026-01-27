#ifndef UI_SCANNER_SELECT
#define UI_SCANNER_SELECT

#include <QComboBox>
#include <QVariant>
#include "../Service/DeviceList.hpp"

namespace ui {
    class CScannerSelect : public QComboBox {
        Q_OBJECT
      public:
        CScannerSelect(QWidget* parent = nullptr);

      private slots:
        void sl_scanners_changed(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items);

      private:
        void display_items();
        void display_items(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items);
    };
}

#endif // !UI_SCANNER_SELECT
