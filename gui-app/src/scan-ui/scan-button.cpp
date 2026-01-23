#include "scan-button.hpp"
#include <QPushButton>
#include <QWidget>

CScanButton::CScanButton(QWidget* parent, ScannerService::Stub& stub) : QPushButton("Scan", parent), m_stub(stub) {}
