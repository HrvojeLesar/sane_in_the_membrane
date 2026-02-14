#ifndef UI_IMAGE_IMAGE_HORIZONTAL_SCROLL
#define UI_IMAGE_IMAGE_HORIZONTAL_SCROLL

#include <qevent.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
namespace sane_in_the_membrane::ui::image {
    class CImageHorizontalScroll : public QScrollArea {
        Q_OBJECT

      public:
        CImageHorizontalScroll(QWidget* parent);

      protected:
        virtual void wheelEvent(QWheelEvent* event) override;
    };
}

#endif // !UI_IMAGE_IMAGE_HORIZONTAL_SCROLL
