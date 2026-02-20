#include "ScanWidget.hpp"

using namespace sane_in_the_membrane::ui::scan;

CScanWidget::CScanWidget(ui::CScannerSelect* scanner_select, QWidget* parent) :
    QWidget(parent), m_scan_button(new CScanButton(scanner_select, this)), m_scan_progress(new CScanProgress(this)), m_layout(new QHBoxLayout(this)) {
    m_layout->addWidget(m_scan_button);
    m_layout->addWidget(m_scan_progress);
}
