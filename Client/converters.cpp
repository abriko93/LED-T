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

QPixmap VisualPixmapConverter::content() const
{
    QImage img = getImage();
    for (int i = 1; i < img.height(); i += 2)
    {
        for (int j = 0; j < img.width() / 2; j++)
        {
            int k = img.width() - j - 1;
            QRgb px = img.pixel(j, i);
            img.setPixel(j, i, img.pixel(k, i));
            img.setPixel(k, i, px);
        }
    }

    return QPixmap::fromImage(img);
}

SimpleTextGRBConverter::SimpleTextGRBConverter()
    : BaseConverter()
    , stream(new QString) // XXX: Leak ?
{
}

void SimpleTextGRBConverter::prepareMetadata(const Image &)
{
    stream.reset();
}

void SimpleTextGRBConverter::convertPixel(const Image &, const QColor &px)
{
    stream << px.green() << " " << px.red() << " " << px.blue() << " ";
}

void convertImage(const Image &img, BaseConverter *converter)
{
    if (img.originalImage().isNull())
        return;

    converter->prepareMetadata(img);
    for (QColor const& px : img)
    {
        QColor realColor(px.red() / 255.0 * img.brightness(),
                         px.green() / 255.0 * img.brightness(),
                         px.blue() / 255.0 * img.brightness());

        converter->convertPixel(img, realColor);
    }
}

GRBConverterV1::GRBConverterV1()
    : BaseConverter()
{}

void GRBConverterV1::cleanup()
{
    data.clear();
    stream = QSharedPointer<QDataStream>(new QDataStream(&data, QIODevice::WriteOnly));
}

void GRBConverterV1::prepareMetadata(const Image &)
{
    cleanup();
    *stream << (quint8)'S';
}

void GRBConverterV1::convertPixel(const Image &, const QColor &px)
{
    *stream << (quint8)px.green() << (quint8)px.red() << (quint8)px.blue();
}

void GRBConverterV2::prepareMetadata(const Image &image)
{
    cleanup();
    quint8 maxSize = ~0;
    if (image.width() > maxSize || image.height() > maxSize) {
        qCritical() << "Too large image:" << image.width() << "x" << image.height() << "(" << maxSize << "max)";
        return;
    }

    getStream() << (quint8)'S' << (quint8)'T' << (quint8)image.width() << (quint8)image.height();
}
