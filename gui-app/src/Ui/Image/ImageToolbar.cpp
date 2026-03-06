#include "ImageToolbar.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <qcoreevent.h>
#include <qdir.h>
#include <qicon.h>
#include <qnamespace.h>
#include <qpalette.h>
#include <qsize.h>

#include "../../Utils/Icons/IconColourizer.hpp"
#include "../../Utils/Icons/IconPaths.hpp"

using namespace sane_in_the_membrane::ui::image;

CImageToolbar::CImageToolbar(std::size_t page_number, QWidget* parent) :
    QWidget(parent), m_btn_move_next(new QPushButton(this)), m_btn_move_prev(new QPushButton(this)), m_btn_rotate_left(new QPushButton(this)),
    m_btn_rotate_right(new QPushButton(this)), m_btn_mirror(new QPushButton(this)), m_btn_delete(new QPushButton(this)), m_page_number(new QLabel(this)) {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);

    set_icons();

    set_page_number(page_number);
    layout->addWidget(m_page_number);

    m_btn_move_prev->setToolTip("Move to previous page");
    layout->addWidget(m_btn_move_prev);

    m_btn_move_next->setToolTip("Move to next page");
    layout->addWidget(m_btn_move_next);

    m_btn_rotate_left->setToolTip("Rotate counter-clockwise");
    layout->addWidget(m_btn_rotate_left);

    m_btn_rotate_right->setToolTip("Rotate clockwise");
    layout->addWidget(m_btn_rotate_right);

    m_btn_mirror->setToolTip("Mirror");
    layout->addWidget(m_btn_mirror);

    m_btn_delete->setToolTip("Delete");
    layout->addWidget(m_btn_delete);

    layout->addStretch();
}

void CImageToolbar::set_page_number(std::size_t page_number) {
    m_page_number->setText(std::format("{}.", page_number).c_str());
}

void CImageToolbar::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ApplicationPaletteChange)
        set_icons();
}

void CImageToolbar::set_icons() {
    auto hints   = QGuiApplication::styleHints();
    auto isLight = hints->colorScheme() == Qt::ColorScheme::Light;

    auto colour = isLight ? Qt::black : Qt::white;
    auto size   = QSize{32, 32};

    m_btn_move_prev->setIcon(icons::colourize_svg(icons::CIcons::ARROW_LEFT, colour, size));
    m_btn_move_next->setIcon(icons::colourize_svg(icons::CIcons::ARROW_RIGHT, colour, size));
    m_btn_rotate_left->setIcon(icons::colourize_svg(icons::CIcons::ARROW_COUNTER_CLOCKWISE, colour, size));
    m_btn_rotate_right->setIcon(icons::colourize_svg(icons::CIcons::ARROW_CLOCKWISE, colour, size));
    m_btn_mirror->setIcon(icons::colourize_svg(icons::CIcons::MIRROR, colour, size));
    m_btn_delete->setIcon(icons::colourize_svg(icons::CIcons::RUBBISH_BIN, colour, size));
}
