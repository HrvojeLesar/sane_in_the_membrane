#ifndef UTILS_ICONS_ICON_COLUORIZER
#define UTILS_ICONS_ICON_COLUORIZER

#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <qsvgrenderer.h>

namespace sane_in_the_membrane::ui::icons {
    QPixmap colourize_svg(const QString& path, const QColor& colour, const QSize& size);
}

#endif // !UTILS_ICONS_ICON_COLORIZER
