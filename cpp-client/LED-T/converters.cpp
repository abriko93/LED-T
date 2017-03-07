#include "converters.h"
#include <image.h>

#include <QDebug>

BaseConverter::BaseConverter()
{
}

PixmapConverter::PixmapConverter()
    : BaseConverter()
{
}

void PixmapConverter::prepareMetadata(const Image &image)
{
    i = j = 0;
    data = QImage(image.width(), image.height(), QImage::Format_RGB888);
}

void PixmapConverter::convertPixel(const Image &img, const QColor &px)
{
    data.setPixel(i, j, px.rgb());
    if (++i >= img.width())
    {
        i = 0;
        ++j;
    }
}

QPixmap PixmapConverter::content() const
{
    return QPixmap::fromImage(data);
}

void convertImage(const Image &img, BaseConverter *converter)
{
    converter->prepareMetadata(img);
    for (QColor const& px : img)
    {
        QColor realColor(px.red() / 255.0 * img.brightness(),
                         px.green() / 255.0 * img.brightness(),
                         px.blue() / 255.0 * img.brightness());

        converter->convertPixel(img, realColor);
    }
}
