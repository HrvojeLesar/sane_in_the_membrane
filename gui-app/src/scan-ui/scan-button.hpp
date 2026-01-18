#pragma once
#ifndef SCAN_BUTTON_H
#define SCAN_BUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QString>

class CScanButton : QPushButton {
  public:
    CScanButton(QWidget* parent = nullptr);
    CScanButton(const QString& text, QWidget* parent = nullptr);
};

#endif // !SCAN_BUTTON_H
