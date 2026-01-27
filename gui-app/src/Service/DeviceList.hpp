#ifndef SERVICE_DEVICE_LIST
#define SERVICE_DEVICE_LIST

#include "scanner/v1/scanner.pb.h"
#include <QComboBox>
#include <QVariant>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>

#include "SynchronizedAccess.hpp"

namespace service {
    using namespace sane_in_the_membrane::utils;

    class CScannerItem : public QVariant {
      public:
        CScannerItem(QString&& scanner_name, QString&& scanner_display_name);
        CScannerItem(const scanner::v1::ScannerInfo& scanner_info);
        ~CScannerItem();

        CScannerItem(const CScannerItem& other);
        CScannerItem(const CScannerItem&& other);

        const QString& scanner_name() const {
            return m_scanner_name;
        }

        const QString& scanner_display_name() const {
            return m_scanner_display_name;
        }

      private:
        QString m_scanner_name;
        QString m_scanner_display_name;
    };

    class CDeviceList : public QObject {
        Q_OBJECT
      public:
        CDeviceList();
        ~CDeviceList();

        void                                              add_scanner_item(const CScannerItem scanner_item);
        void                                              set_scanner_items(const std::vector<std::shared_ptr<CScannerItem>>& items);
        const std::vector<std::shared_ptr<CScannerItem>>& get_scanners() const;
        void                                              clear();

      private slots:
        void sl_get_scanners_failed();
        void sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response);

      signals:
        void sig_scanners_changed(const std::vector<std::shared_ptr<CScannerItem>>&);

      private:
        UniqueAccess<std::vector<std::shared_ptr<CScannerItem>>> m_scanners{};
    };

    inline std::unique_ptr<CDeviceList> g_device_list{nullptr};
}

#endif // !SERVICE_DEVICE_LIST
