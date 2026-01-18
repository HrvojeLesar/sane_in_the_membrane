#include "scan-button.hpp"
#include <QPushButton>
#include <QWidget>

CScanButton::CScanButton(QWidget* parent) : QPushButton("Scan", parent) {}
CScanButton::CScanButton(const QString& text, QWidget* parent) : QPushButton("Scan", parent) {}
