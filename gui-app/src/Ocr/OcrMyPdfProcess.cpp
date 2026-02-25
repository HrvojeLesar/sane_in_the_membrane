#include "OcrMyPdfProcess.hpp"
#include <GLogger.hpp>
#include <QObject>

using namespace sane_in_the_membrane::ocr;

COcrMyPdfProcess::COcrMyPdfProcess() : m_process(), m_notifier(m_process) {
    int result  = std::system("ocrmypdf --version");
    m_installed = result == 0;
}

bool COcrMyPdfProcess::is_ocrmypdf_installed() const {
    return m_installed;
}

void COcrMyPdfProcess::ocr(QString& input_file) {
    stop();
    start(input_file);
}

void COcrMyPdfProcess::start(QString& input_file) {
    QStringList arguments{"--force-ocr", input_file, input_file};

    m_notifier.connect();
    m_process.start("ocrmypdf", arguments);
}

void COcrMyPdfProcess::stop() {
    if (m_process.state() == QProcess::Running || m_process.state() == QProcess::Starting)
        m_process.kill();

    m_notifier.disconnect_connections();
}

COcrMyPdfNotifier::COcrMyPdfNotifier(QProcess& process) : m_process(process) {}

void COcrMyPdfNotifier::disconnect_connections() {
    QObject::disconnect(this);
}

void COcrMyPdfNotifier::connect() {
    QObject::connect(&m_process, &QProcess::started, this, [this]() {});
    QObject::connect(&m_process, &QProcess::readyReadStandardError, this, [this]() { log::warn(m_process.readAllStandardError()); });
    QObject::connect(&m_process, &QProcess::readyReadStandardOutput, this, [this]() { log::warn(m_process.readAllStandardOutput()); });
}
