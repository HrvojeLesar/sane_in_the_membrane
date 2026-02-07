#ifndef UI_IMAGE_IMAGE_TOOLBAR
#define UI_IMAGE_IMAGE_TOOLBAR

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <qlabel.h>

namespace sane_in_the_membrane::ui::image {
    class CImageToolbar : public QWidget {
        Q_OBJECT

      public:
        CImageToolbar(uint32_t page_number, QWidget* parent = nullptr);

      public:
        void set_page_number(uint32_t page_number);

      public:
        QPushButton* const m_btn_move_next;
        QPushButton* const m_btn_move_prev;
        QPushButton* const m_btn_rotate_left;
        QPushButton* const m_btn_rotate_right;
        QPushButton* const m_btn_mirror;
        QPushButton* const m_btn_delete;
        QLabel* const      m_page_number;
    };

}

#endif // !UI_IMAGE_IMAGE_TOOLBAR
