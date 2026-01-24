#ifndef UI_SCANNER_SELECT
#define UI_SCANNER_SELECT

#include <QComboBox>
#include <QVariant>

namespace ui {
    class CScannerSelect : public QComboBox {
        Q_OBJECT
      public:
        CScannerSelect(QWidget* parent = nullptr);

      private slots:
        void sl_scanners_changed();

      private:
        void display_items();
    };
}

#endif // !UI_SCANNER_SELECT
