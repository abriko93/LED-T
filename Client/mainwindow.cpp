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
    configurator.registerConfigurableWidget(ui->saveAsImgEdt);
    configurator.registerConfigurableWidget(ui->saveAsTextLineEdt);
    configurator.registerConfigurableWidget(ui->saveAsBinaryLineEdt);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    reloadImage();

    connect(ui->fromLineEdit, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
    connect(ui->imgHeiLineEdt, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
    connect(ui->imgWidthLineEdt, SIGNAL(editingFinished()), this, SLOT(on_reloadImageNeeded()));
}

void MainWindow::on_fromToolBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "File to convert");
    ui->fromLineEdit->setText(fileName);

    reloadImage();
}

Image MainWindow::getImage()
{
    Image img(QPixmap(ui->fromLineEdit->text()), this);
    Image invalid(QPixmap(), this);

    bool ok = false;
    int w = ui->imgWidthLineEdt->text().toInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid text found in imgWidth field:" << ui->imgWidthLineEdt->text();
        return invalid;
    }
    img.setWidth(w);

    int h = ui->imgHeiLineEdt->text().toInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid text found in imgWidth field:" << ui->imgWidthLineEdt->text();
        return invalid;
    }
    img.setHeight(h);

    if (w <= 0 || h <= 0)
        return invalid;

    img.setBrightness(ui->brightnessSlider->value());

    return img;
}

void MainWindow::reloadImage()
{
    QList<QGraphicsItem *> items = ui->graphicsView->scene()->items();
    for (QGraphicsItem *item : items)
    {
        ui->graphicsView->scene()->removeItem(item);
    }

    PixmapConverter converter;
    convertImage(getImage(), &converter);

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

void MainWindow::on_saveAsImgToolBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as image", "", "*.png");
    ui->saveAsImgEdt->setText(fileName);
}

void MainWindow::on_saveAsImgBtn_clicked()
{
    if (ui->saveAsImgEdt->text() == "")
    {
        QMessageBox::warning(this, "Error", "Can't save: empty file name!");
        return;
    }

    PixmapConverter converter;
    convertImage(getImage(), &converter);

    QImage img = converter.content().toImage();
    img.save(ui->saveAsImgEdt->text(), "PNG");

    QMessageBox::information(this, "Success!", "Saved!");
}

void MainWindow::on_saveAsTextToolBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as text");
    ui->saveAsImgEdt->setText(fileName);
}

void MainWindow::on_saveAsTextBtn_clicked()
{
    if (ui->saveAsTextLineEdt->text() == "")
    {
        QMessageBox::warning(this, "Error", "Can't save: empty file name!");
        return;
    }

    SimpleTextGRBConverter converter;
    convertImage(getImage(), &converter);

    QFile file(ui->saveAsTextLineEdt->text());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Can't open file `" + file.fileName() + "`: " + file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream << converter.string();

    QMessageBox::information(this, "Success!", "Saved!");
}

void MainWindow::on_saveAsBinaryToolBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as binary");
    ui->saveAsBinaryLineEdt->setText(fileName);
}

void MainWindow::on_saveAsBinaryBtn_clicked()
{
    if (ui->saveAsBinaryLineEdt->text() == "")
    {
        QMessageBox::warning(this, "Error", "Can't save: empty file name!");
        return;
    }

    GRBConverterV1 converter;
    convertImage(getImage(), &converter);

    QFile file(ui->saveAsBinaryLineEdt->text());
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Error", "Can't open file `" + file.fileName() + "`: " + file.errorString());
        return;
    }

    file.write(converter.content());

    QMessageBox::information(this, "Success!", "Saved!");
}
