#include "SemaphoneWidget.hpp"
#include "../Utils/Globals.hpp"
#include "../Utils/mDNS/mDnsAutoFind.hpp"
#include <QPainter>
#include <iostream>
#include <qcolor.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <QMouseEvent>

using namespace sane_in_the_membrane::ui;

CSemaphoneWidget::CSemaphoneWidget(QWidget* parent) : QWidget(parent), m_status(ESemaphoneStatus::Red) {
    QObject::connect(&utils::Globals::get_instance().proxies()->m_change_state_watcher_proxy, &utils::proxy::CChangeStateWatcher::sig_channel_state_changed, this,
                     &CSemaphoneWidget::sl_channel_state_changed);
    QObject::connect(&utils::Globals::get_instance().proxies()->m_change_state_watcher_proxy, &utils::proxy::CChangeStateWatcher::sig_stopping_auto_discovery, this,
                     &CSemaphoneWidget::sl_stopping_auto_discovery);

    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_discovering, this,
                     &CSemaphoneWidget::sl_discovering);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_mdns_discovered, this,
                     &CSemaphoneWidget::sl_mdns_discovered);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_discover_failed, this,
                     &CSemaphoneWidget::sl_discover_failed);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_query_failed, this,
                     &CSemaphoneWidget::sl_query_failed);

    setMinimumSize({30, 30});
}

void CSemaphoneWidget::sl_channel_state_changed(sane_in_the_membrane::service::CChangeStateWatcher::CChannelState state) {
    switch (state.get()) {
        case GRPC_CHANNEL_IDLE:
        case GRPC_CHANNEL_READY: set_status(ESemaphoneStatus::Green); break;
        case GRPC_CHANNEL_CONNECTING: set_status(ESemaphoneStatus::Yellow); break;
        case GRPC_CHANNEL_TRANSIENT_FAILURE:
        case GRPC_CHANNEL_SHUTDOWN: set_status(ESemaphoneStatus::Red); break;
    }
}
void CSemaphoneWidget::sl_discovering() {
    set_status(ESemaphoneStatus::Yellow);
}

void CSemaphoneWidget::sl_mdns_discovered(const std::vector<sane_in_the_membrane::utils::mdns::SQueryResult>& discovered_connections) {
    // Write some message
    if (discovered_connections.empty())
        set_status(ESemaphoneStatus::Red);
}

void CSemaphoneWidget::sl_discover_failed(const std::string& error) {
    // Write some message
    set_status(ESemaphoneStatus::Red);
}

void CSemaphoneWidget::sl_query_failed(const std::string& error) {
    // Write some message
    set_status(ESemaphoneStatus::Red);
}

void CSemaphoneWidget::set_status(ESemaphoneStatus status) {
    m_status = status;
    update();
}

void CSemaphoneWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QColor color;
    switch (m_status) {
        case Green: color = Qt::GlobalColor::green; break;
        case Yellow: color = QColor(255, 170, 0); break;
        case Red: color = Qt::GlobalColor::red; break;
    }

    QRect circle{0, (height() - size().width()) / 2, size().width(), size().height()};

    painter.setBrush(color);
    painter.setPen(Qt::GlobalColor::black);
    painter.drawEllipse(circle);
}

void CSemaphoneWidget::mouseReleaseEvent(QMouseEvent* event) {
    sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance().discover();
}

void CSemaphoneWidget::sl_stopping_auto_discovery() {
    std::cout << "Stopping auto discovery\n";
    // Write some message
}
