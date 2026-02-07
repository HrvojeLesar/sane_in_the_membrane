#include "ImageToolbar.hpp"

using namespace sane_in_the_membrane::ui::image;

CImageToolbar::CImageToolbar(uint32_t page_number, QWidget* parent) :
    QWidget(parent), m_btn_move_next(new QPushButton(this)), m_btn_move_prev(new QPushButton(this)), m_btn_rotate_left(new QPushButton(this)),
    m_btn_rotate_right(new QPushButton(this)), m_btn_mirror(new QPushButton(this)), m_btn_delete(new QPushButton(this)), m_page_number(new QLabel(this)) {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);

    set_page_number(page_number);
    layout->addWidget(m_page_number);

    m_btn_move_prev->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    // m_btn_move_prev->setIconSize(QSize(24, 24));
    // m_btn_move_prev->setFixedSize(36, 36);
    m_btn_move_prev->setToolTip("Move to previous page");
    layout->addWidget(m_btn_move_prev);

    m_btn_move_next->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    // m_btn_move_next->setIconSize(QSize(24, 24));
    // m_btn_move_next->setFixedSize(36, 36);
    m_btn_move_next->setToolTip("Move to next page");
    layout->addWidget(m_btn_move_next);

    m_btn_rotate_left->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    // m_btn_rotate_left->setIconSize(QSize(24, 24));
    // m_btn_rotate_left->setFixedSize(36, 36);
    m_btn_rotate_left->setToolTip("Rotate counter-clockwise");
    layout->addWidget(m_btn_rotate_left);

    m_btn_rotate_right->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    // m_btn_rotate_right->setIconSize(QSize(24, 24));
    // m_btn_rotate_right->setFixedSize(36, 36);
    m_btn_rotate_right->setToolTip("Rotate clockwise");
    layout->addWidget(m_btn_rotate_right);

    m_btn_mirror->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    // m_btn_mirror->setIconSize(QSize(24, 24));
    // m_btn_mirror->setFixedSize(36, 36);
    m_btn_mirror->setToolTip("Mirror");
    layout->addWidget(m_btn_mirror);

    m_btn_delete->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    // m_btn_delete->setIconSize(QSize(24, 24));
    // m_btn_delete->setFixedSize(36, 36);
    m_btn_delete->setToolTip("Delete");
    layout->addWidget(m_btn_delete);

    layout->addStretch();
}

void CImageToolbar::set_page_number(uint32_t page_number) {
    m_page_number->setText(std::format("{}.", page_number).c_str());
}
