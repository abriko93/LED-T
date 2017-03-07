#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QPixmap>

class Image;

const QColor defaultColor = Qt::black;

class colorIterator {
public:
    colorIterator(const Image *img, bool atEnd = false);
    bool operator ==(colorIterator const& other) const;
    bool operator !=(colorIterator const& other) const { return !(*this == other); }
    QColor operator *() const;
    colorIterator &operator ++();

protected:
    colorIterator(const Image *img, int i, int j);

private:
    int i, j;
    const Image *img;
};

class Image : public QObject
{
    Q_OBJECT
public:
    explicit Image(QPixmap const& pixmap, QObject *parent = 0);
    Image(Image const& other);

public:
    int width() const { return w; }
    int height() const { return h; }
    int brightness() const { return imgBrightness; }

    void setWidth(int w);
    void setHeight(int h);

    void setBrightness(int b);

public:
    colorIterator begin() const;
    colorIterator end() const;

    QPixmap const& originalImage() const { return origImage; }

signals:

public slots:

private:
    QPixmap origImage;
    int w, h;
    int imgBrightness;
};

#endif // IMAGE_H
