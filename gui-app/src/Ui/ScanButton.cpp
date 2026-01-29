#include "ScanButton.hpp"
#include "ScannerSelect.hpp"
#include <iostream>
#include <qobject.h>
#include "../Readers/ScanResponseReader.hpp"

using namespace sane_in_the_membrane::ui;

CScanButton::CScanButton(::ui::CScannerSelect* scanner_select, QWidget* parent) : QPushButton("Scan", parent), m_scanner_select(scanner_select) {
    QObject::connect(this, &CScanButton::clicked, this, &CScanButton::sl_clicked);
    QObject::connect(reader::g_scan_response_reader.get(), &reader::CScanResponseReader::sig_done, this, &CScanButton::sl_sig_done);
}

void CScanButton::sl_clicked() {
    setDisabled(true);
    const auto scanner_data = m_scanner_select->currentData();
    std::cout << scanner_data->scanner_name().toStdString() << scanner_data->scanner_display_name().toStdString() << "\n";

    m_request.set_scanner_name(scanner_data->scanner_name().toStdString());
    reader::g_scan_response_reader->scan(m_request);
}
void CScanButton::sl_sig_done(const grpc::Status& status) {
    setDisabled(false);
}
