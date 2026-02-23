#include "IconColourizer.hpp"

using namespace sane_in_the_membrane::ui;

QPixmap icons::colourize_svg(const QString& path, const QColor& colour, const QSize& size) {
    QSvgRenderer renderer{path};
    QPixmap      pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), colour);
    painter.end();

    return pixmap;
}
