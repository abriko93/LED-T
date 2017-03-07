#include "image.h"

Image::Image(const QPixmap &pixmap, QObject *parent)
    : QObject(parent)
    , origImage(pixmap)
    , imgBrightness(255)
{
}

Image::Image(Image const& other)
    : QObject(other.parent())
    , origImage(other.origImage) // transformation will not be copied here
    , imgBrightness(255)
{
}

void Image::setWidth(int w)
{
    this->w = w;
}

void Image::setHeight(int h)
{
    this->h = h;
}

void Image::setBrightness(int b)
{
    this->imgBrightness = b;
}

colorIterator::colorIterator(const Image *img, bool atEnd)
    : i(0)
    , j(0)
    , img(img)
{
    if (atEnd)
    {
        i = img->width();
        j = img->height() - 1;
    }
}

colorIterator::colorIterator(const Image *img, int i, int j)
    : i(i)
    , j(j)
    , img(img)
{}

bool colorIterator::operator ==(colorIterator const& other) const
{
    return img == other.img && i == other.i && j == other.j;
}

colorIterator &colorIterator::operator ++()
{
    if (++i >= img->width() && j < img->height() - 1)
    {
        i = 0;
        j++;
    }

    return *this;
}

QColor colorIterator::operator *() const
{
    int pxx = i, pxy = j;

    int xMargin = (img->width() - img->originalImage().width()) / 2; // int devision
    if (xMargin > 0)
    {
        // image is smaller then needed. Center it

        // left margin
        if (pxx < xMargin)
            return defaultColor;

        // right margin
        if (pxx > img->originalImage().width() + xMargin - 1)
            return defaultColor;

        pxx -= xMargin; // real image here
    } else if (xMargin < 0) {
        // image is larger than needed. Crop it
        pxx -= xMargin; // negative xMargin here => minus required
    }

    if (pxx == img->originalImage().width())
        return defaultColor; // integer devision fixup

    int yMargin = (img->height() - img->originalImage().height()) / 2; // int devision
    if (yMargin > 0)
    {
        // image is smaller then needed. Center it

        // upper margin
        if (pxy < yMargin)
            return defaultColor;

        // bottom margin
        if (pxy > img->originalImage().height() + yMargin - 1)
            return defaultColor;

        pxy -= yMargin;
    } else if (yMargin < 0) {
        // image is larger then needed. Crop it
        pxy -= yMargin; // negative yMargin here => minus required
    }

    if (pxy == img->originalImage().height())
        return defaultColor; // integer devision fixup

    return img->originalImage().toImage().pixel(pxx, pxy);
}

colorIterator Image::begin() const
{
    return colorIterator(this);
}

colorIterator Image::end() const
{
    return colorIterator(this, true);
}
