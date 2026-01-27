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
        void sl_scanners_changed(const std::vector<std::shared_ptr<service::CScannerItem>>&);

      private:
        void display_items();
        void display_items(const std::vector<std::shared_ptr<service::CScannerItem>>&);
    };
}

#endif // !UI_SCANNER_SELECT
