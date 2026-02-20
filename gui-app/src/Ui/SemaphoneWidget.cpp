#include "SemaphoneWidget.hpp"
#include "../Utils/Globals.hpp"
#include "../Utils/mDNS/mDnsAutoFind.hpp"
#include <QPainter>
#include <qcolor.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <QMouseEvent>
#include <GLogger.hpp>

using namespace sane_in_the_membrane::ui;

CSemaphoreLight::CSemaphoreLight(QWidget* parent) : QWidget(parent), m_status(ESemaphoneStatus::Red) {
    setMinimumSize({30, 30});
}

void CSemaphoreLight::set_status(ESemaphoneStatus status) {
    m_status = status;
    update();
}

void CSemaphoreLight::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QColor color;
    switch (m_status) {
        case Green: color = Qt::GlobalColor::green; break;
        case Yellow: color = QColor(255, 170, 0); break;
        case Red: color = Qt::GlobalColor::red; break;
    }

    QRectF circle{size().width() / 4.0, size().height() / 4.0, size().width() / 2.5, size().height() / 2.5};

    painter.setBrush(color);
    painter.setPen(Qt::GlobalColor::black);
    painter.drawEllipse(circle);
}

void CSemaphoreLight::mouseReleaseEvent(QMouseEvent* event) {
    sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance().discover();
}

CSemaphoreWidget::CSemaphoreWidget(QWidget* parent) : QWidget(parent), m_semaphore_light(new CSemaphoreLight()), m_horizontal_layout(new QHBoxLayout(this)), m_text(new QLabel()) {
    m_horizontal_layout->addWidget(m_semaphore_light);
    m_horizontal_layout->addWidget(m_text);

    QObject::connect(&utils::Globals::get_instance().proxies()->m_change_state_watcher_proxy, &utils::proxy::CChangeStateWatcher::sig_channel_state_changed, this,
                     &CSemaphoreWidget::sl_channel_state_changed);
    QObject::connect(&utils::Globals::get_instance().proxies()->m_change_state_watcher_proxy, &utils::proxy::CChangeStateWatcher::sig_stopping_auto_discovery, this,
                     &CSemaphoreWidget::sl_stopping_auto_discovery);

    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_discovering, this,
                     &CSemaphoreWidget::sl_discovering);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_mdns_discovered, this,
                     &CSemaphoreWidget::sl_mdns_discovered);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_discover_failed, this,
                     &CSemaphoreWidget::sl_discover_failed);
    QObject::connect(&sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance(), &sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::sig_query_failed, this,
                     &CSemaphoreWidget::sl_query_failed);
}

void CSemaphoreWidget::sl_channel_state_changed(sane_in_the_membrane::service::CChangeStateWatcher::CChannelState state) {
    switch (state.get()) {
        case GRPC_CHANNEL_IDLE:
        case GRPC_CHANNEL_READY: m_semaphore_light->set_status(ESemaphoneStatus::Green); break;
        case GRPC_CHANNEL_CONNECTING: m_semaphore_light->set_status(ESemaphoneStatus::Yellow); break;
        case GRPC_CHANNEL_TRANSIENT_FAILURE:
        case GRPC_CHANNEL_SHUTDOWN: m_semaphore_light->set_status(ESemaphoneStatus::Red); break;
    }
}
void CSemaphoreWidget::sl_discovering() {
    m_semaphore_light->set_status(ESemaphoneStatus::Yellow);
    m_text->setText("Connecting...");
}

void CSemaphoreWidget::sl_mdns_discovered(const std::vector<sane_in_the_membrane::utils::mdns::SQueryResult>& discovered_connections) {
    if (discovered_connections.empty()) {
        m_semaphore_light->set_status(ESemaphoneStatus::Red);
        m_text->setText("Unable to find scanner server");
        log::debug("Unable to find scanner server");
    } else {
        m_semaphore_light->set_status(ESemaphoneStatus::Green);
        m_text->setText("Connected");
        log::debug("Connected");
    }
}

void CSemaphoreWidget::sl_discover_failed(const std::string& error) {
    m_semaphore_light->set_status(ESemaphoneStatus::Red);
    m_text->setText("Failed to discover scanner server");
    log::warn("Failed to discover scanner server");
}

void CSemaphoreWidget::sl_query_failed(const std::string& error) {
    m_semaphore_light->set_status(ESemaphoneStatus::Red);
    m_text->setText("Failed to query scanner server");
    log::warn("Failed to query scanner server");
}

void CSemaphoreWidget::sl_stopping_auto_discovery() {
    m_semaphore_light->set_status(ESemaphoneStatus::Red);
    m_text->setText("Server auto discovery stopped, click the red icon to try discovering scanner server");
}
