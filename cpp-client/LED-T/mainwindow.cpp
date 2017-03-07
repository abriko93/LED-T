#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QGraphicsPixmapItem>

#include "converters.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    prepareForm();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::prepareForm()
{
    ui->graphicsView->setScene(new QGraphicsScene(this));

    ui->imgHeiLineEdt->setValidator(new QIntValidator(1, 1024, ui->imgHeiLineEdt));
    ui->imgWidthLineEdt->setValidator(new QIntValidator(1, 1024, ui->imgWidthLineEdt));

    configurator.registerConfigurableWidget(ui->fromLineEdit);
    configurator.registerConfigurableWidget(ui->imgHeiLineEdt);
    configurator.registerConfigurableWidget(ui->imgWidthLineEdt);
    configurator.registerConfigurableWidget(ui->brightnessSlider);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    reloadImage();

    connect(ui->fromLineEdit, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
    connect(ui->imgHeiLineEdt, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
    connect(ui->imgWidthLineEdt, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
}

void MainWindow::setFile(QString title, QLineEdit *edt)
{
    QString fileName = QFileDialog::getOpenFileName(this, title);
    edt->setText(fileName);
}

void MainWindow::on_fromToolBtn_clicked()
{
    setFile("File to convert", ui->fromLineEdit);
    reloadImage();
}

QString convertImage(QFile &from, QFile &to)
{
    QImage img;
    if (!img.load(&from, "PNG")) {
        return "Can't load image";
    }

    int w = img.width();
    int h = img.height();

    QTextStream out(&to);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            QRgb color = img.pixel(j, i);
            int r = qRed(color);
            int g = qGreen(color);
            int b = qBlue(color);

            // Uncomment following line if hexadecimal out is needed
            //hex(out) << g << " " << r << " " << b << " ";

            out << g << " " << r << " " << b << " ";
        }
    }

    return "";
}

/*
void MainWindow::on_convertBtn_clicked()
{
    if (this->ui->fromLineEdit->text() == "" || this->ui->toLineEdit->text() == "") {
        QMessageBox::critical(this, "error", "Both filenames are required");
        return;
    }

    QFile fromFile(this->ui->fromLineEdit->text());
    if (!fromFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Can't open file", "Can't open file `" +
                              fromFile.fileName() + "` for reading: " + fromFile.errorString());
        return;
    }

    QFile toFile(this->ui->toLineEdit->text());
    if (!toFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Can't open file", "Can't open file `" +
                              toFile.fileName() + "` for writing: " + toFile.errorString());
        return;
    }

    QString convertResult = convertImage(fromFile, toFile);
    if (convertResult != "") {
        QMessageBox::critical(this, "convertion failed", "can't convert image: " + convertResult);
        return;
    }

    QMessageBox::information(this, "Success!", "Image successfully converted");
}
*/

void MainWindow::reloadImage()
{
    QList<QGraphicsItem *> items = ui->graphicsView->scene()->items();
    for (QGraphicsItem *item : items)
    {
        ui->graphicsView->scene()->removeItem(item);
    }

    Image img(QPixmap(ui->fromLineEdit->text()), this);

    bool ok = false;
    int w = ui->imgWidthLineEdt->text().toInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid text found in imgWidth field:" << ui->imgWidthLineEdt->text();
        return;
    }
    img.setWidth(w);

    int h = ui->imgHeiLineEdt->text().toInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid text found in imgWidth field:" << ui->imgWidthLineEdt->text();
        return;
    }
    img.setHeight(h);

    if (w <= 0 || h <= 0)
        return;

    img.setBrightness(ui->brightnessSlider->value());

    PixmapConverter converter;
    convertImage(img, &converter);

    pixmapItem = QSharedPointer<QGraphicsPixmapItem>(new QGraphicsPixmapItem(converter.content()));
    ui->graphicsView->scene()->addItem(&*pixmapItem);

    resizeImage();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resizeImage();
}

void MainWindow::resizeImage()
{
    QTransform transformation;
    transformation.scale((qreal)ui->graphicsView->width() / (qreal)pixmapItem->boundingRect().width(),
                         (qreal)ui->graphicsView->height() / (qreal)pixmapItem->boundingRect().height());

    pixmapItem->setTransform(transformation);
    ui->graphicsView->setSceneRect(ui->graphicsView->rect());
}

void MainWindow::on_reloadImageNeeded()
{
    reloadImage();
}

void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    ui->brightnessLbl->setText(QString::number(value));
    reloadImage();
}
