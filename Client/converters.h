#ifndef BASECONVERTER_H
#define BASECONVERTER_H

#include <QImage>
#include <QTextStream>

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

class SimpleTextGRBConverter : public BaseConverter
{
public:
    SimpleTextGRBConverter();

    virtual void prepareMetadata(Image const& image);
    virtual void convertPixel(Image const& img, QColor const& px);

    QString string() const { return *stream.string(); }

protected:
    QTextStream &content() { return stream; }

private:
    QTextStream stream;
};

class TextGRBConverterV1 : public SimpleTextGRBConverter
{
public:
    TextGRBConverterV1();

    virtual void prepareMetadata(Image const& image);
};

void convertImage(Image const& img, BaseConverter *converter);

#endif // BASECONVERTER_H
