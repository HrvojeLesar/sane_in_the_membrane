#include "ErrorDialogue.hpp"
#include <qapplication.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qobject.h>
#include <GLogger.hpp>
#include <qwidget.h>

using namespace sane_in_the_membrane::utils;

CErrorDialogue::CErrorDialogue(QWidget* parent) : QErrorMessage(parent) {
    QCheckBox* show_again_checkbox = findChild<QCheckBox*>();
    if (show_again_checkbox)
        show_again_checkbox->setVisible(false);

    QObject::connect(this, &CErrorDialogue::finished, [this]() { delete this; });
}

void CErrorDialogue::show_message(QString& message, QWidget* parent) {
    if (parent == nullptr)
        parent = QApplication::activeWindow();

    auto d = new CErrorDialogue{parent};
    d->showMessage(message);
}

void CErrorDialogue::show_message(QString&& message, QWidget* parent) {
    show_message(message, parent);
}
