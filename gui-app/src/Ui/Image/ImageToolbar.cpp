#include "ImageToolbar.hpp"

using namespace sane_in_the_membrane::ui::image;

CImageToolbar::CImageToolbar(QWidget* parent) :
    QWidget(parent), m_btn_move_next(new QPushButton(this)), m_btn_move_prev(new QPushButton(this)), m_btn_rotate_left(new QPushButton(this)),
    m_btn_rotate_right(new QPushButton(this)), m_btn_mirror(new QPushButton(this)), m_btn_delete(new QPushButton(this)) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);

    m_btn_rotate_left->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    // m_btn_rotate_left->setIconSize(QSize(24, 24));
    // m_btn_rotate_left->setFixedSize(36, 36);
    m_btn_rotate_left->setToolTip("Rotate counter-clockwise");
    layout->addWidget(m_btn_rotate_left);

    m_btn_rotate_right->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    // m_btn_rotate_right->setIconSize(QSize(24, 24));
    // m_btn_rotate_right->setFixedSize(36, 36);
    m_btn_rotate_right->setToolTip("Rotate clockwise");
    layout->addWidget(m_btn_rotate_right);

    m_btn_mirror->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
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
