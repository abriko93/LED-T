#ifndef BASECONVERTER_H
#define BASECONVERTER_H

#include <QImage>
#include <QTextStream>
#include <QSharedPointer>

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
    virtual ~PixmapConverter() {}

    virtual void prepareMetadata(Image const& image);
    virtual void convertPixel(Image const& img, QColor const& px);

    virtual QPixmap content() const;

protected:
    QImage const& getImage() const { return data; }

private:
    int i, j;
    QImage data;
};

class VisualPixmapConverter : public PixmapConverter
{
public:
    VisualPixmapConverter() : PixmapConverter() {}

    virtual QPixmap content() const;
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

class GRBConverterV1 : public BaseConverter
{
public:
    GRBConverterV1();

    virtual void prepareMetadata(Image const& image);
    virtual void convertPixel(Image const& img, QColor const& px);

    QByteArray content() const {
        QByteArray newArray(data);
        for (int i = 0; i < 512; ++i)
            newArray.append('\0');
        return newArray;
    }
    /*
    QByteArray const& content() const { return data; }
    */

    virtual ~GRBConverterV1() {}

protected:
    QDataStream &getStream() { return *stream; }
    void cleanup();

private:
    QSharedPointer<QDataStream> stream;
    QByteArray data;
};

class GRBConverterV2 : public GRBConverterV1
{
public:
    GRBConverterV2() : GRBConverterV1() {}

    virtual void prepareMetadata(Image const& image);
};

void convertImage(Image const& img, BaseConverter *converter);

#endif // BASECONVERTER_H
