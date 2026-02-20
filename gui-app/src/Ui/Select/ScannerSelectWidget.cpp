#include "ScannerSelectWidget.hpp"

using namespace sane_in_the_membrane::ui::select;

CScannerSelectWidget::CScannerSelectWidget(QWidget* parent) :
    QWidget(parent), m_layout(new QHBoxLayout(this)), m_refresh_button(new CRefreshButton()), m_scanner_select(new CScannerSelect()) {
    m_layout->addWidget(m_scanner_select);
    m_layout->addWidget(m_refresh_button);
}
CScannerSelect* CScannerSelectWidget::get_scanner_select() {
    return m_scanner_select;
}
