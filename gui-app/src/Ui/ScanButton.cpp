#include "ScanButton.hpp"
#include "ScannerSelect.hpp"
#include <iostream>

using namespace sane_in_the_membrane::ui;

CScanButton::CScanButton(::ui::CScannerSelect* scanner_select, QWidget* parent) : QPushButton("Scan", parent), m_scanner_select(scanner_select) {
    QObject::connect(this, &CScanButton::clicked, this, &CScanButton::sl_clicked);
}

void CScanButton::sl_clicked() {
    const auto scanner_data = m_scanner_select->currentData();
    std::cout << scanner_data->scanner_name().toStdString() << scanner_data->scanner_display_name().toStdString() << "\n";
}
