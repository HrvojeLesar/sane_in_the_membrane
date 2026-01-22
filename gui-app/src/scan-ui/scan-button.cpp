#include "scan-button.hpp"
#include <QPushButton>
#include <QWidget>

CScanButton::CScanButton(QWidget* parent, GreeterService::Stub& stub) : QPushButton("Scan", parent), stub(stub) {}
