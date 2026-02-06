#ifndef UI_IMAGE_IMAGE_TOOLBAR
#define UI_IMAGE_IMAGE_TOOLBAR

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

namespace sane_in_the_membrane::ui::image {
    class CImageToolbar : public QWidget {
        Q_OBJECT

      public:
        CImageToolbar(QWidget* parent = nullptr);

      public:
        QPushButton* const m_btn_move_next;
        QPushButton* const m_btn_move_prev;
        QPushButton* const m_btn_rotate_left;
        QPushButton* const m_btn_rotate_right;
        QPushButton* const m_btn_mirror;
        QPushButton* const m_btn_delete;
    };

}

#endif // !UI_IMAGE_IMAGE_TOOLBAR
