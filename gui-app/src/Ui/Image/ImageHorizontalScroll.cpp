#include "ImageHorizontalScroll.hpp"
#include <QScrollBar>

using namespace sane_in_the_membrane::ui::image;

CImageHorizontalScroll::CImageHorizontalScroll(QWidget* parent) : QScrollArea(parent) {}

void CImageHorizontalScroll::wheelEvent(QWheelEvent* event) {
    if (!verticalScrollBar()->isVisible()) {
        int y_delta = event->angleDelta().y();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - y_delta);
        event->accept();
    } else {
        QScrollArea::wheelEvent(event);
    }
}
