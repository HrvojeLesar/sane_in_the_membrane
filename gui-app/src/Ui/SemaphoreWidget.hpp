#ifndef UI_SEMAPHONE_WIDGET
#define UI_SEMAPHONE_WIDGET

#include <qboxlayout.h>
#include <qevent.h>
#include <qlabel.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "../Service/ChannelStateChangeService.hpp"
#include "../Utils/mDNS/mDns.hpp"

namespace sane_in_the_membrane::ui {
    enum ESemaphoneStatus {
        Green,
        Yellow,
        Red,
    };

    class CSemaphoreLight : public QWidget {
        Q_OBJECT

      public:
        CSemaphoreLight(QWidget* parent = nullptr);

        void set_status(ESemaphoneStatus status);

      signals:
        void click();

      protected:
        void paintEvent(QPaintEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

      private:
        ESemaphoneStatus m_status;
    };

    class CSemaphoreWidget : public QWidget {
        Q_OBJECT
      public:
        CSemaphoreWidget(QWidget* parent = nullptr);

        static constexpr const char* AUTODISCOVER_STOPPED_MESSAGE{"Server auto discovery stopped, click the red icon to try discovering scanner server"};
        static constexpr const char* FAILED_TO_DISCOVER_SCANNER_MESSAGE{"Failed to discover scanner server"};
        static constexpr const char* FAILED_TO_DISCOVER_QUERY_MESSAGE{"Failed to query scanner server"};
        static constexpr const char* UNABLE_TO_FIND_SCANNER_SERVER_MESSAGE{"Unable to find scanner server"};
        static constexpr const char* CONNECTED_MESSAGE{"Connected"};
        static constexpr const char* CONNECTING_MESSAGE{"Connecting..."};

      private slots:
        void sl_channel_state_changed(sane_in_the_membrane::service::CChangeStateWatcher::CChannelState state);
        void sl_discovering();
        void sl_mdns_discovered(const std::vector<sane_in_the_membrane::utils::mdns::SQueryResult>& discovered_connections);
        void sl_discover_failed(const std::string& error);
        void sl_query_failed(const std::string& error);
        void sl_stopping_auto_discovery();
        void sl_rediscover();

      private:
        CSemaphoreLight* const m_semaphore_light;
        QHBoxLayout* const     m_horizontal_layout;
        QLabel* const          m_text;

        bool                   m_stopped_auto_discovery;
    };
}

#endif // !UI_SEMAPHONE_WIDGET
