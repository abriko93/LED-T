#ifndef BASECONVERTER_H
#define BASECONVERTER_H

#include <QImage>

class Image;

class BaseConverter
{
public:
    BaseConverter();

    virtual void prepareMetadata(Image const& img) = 0;
    virtual void convertPixel(Image const &img, QColor const& px) = 0;
};

class PixmapConverter : public BaseConverter
{
public:
    PixmapConverter();

    virtual void prepareMetadata(Image const& image);
    virtual void convertPixel(Image const& img, QColor const& px);

    QPixmap content() const;

private:
    int i, j;
    QImage data;
};

void convertImage(Image const& img, BaseConverter *converter);

#endif // BASECONVERTER_H
