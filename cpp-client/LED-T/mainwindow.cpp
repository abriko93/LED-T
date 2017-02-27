#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QIODevice>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setFile(QString title, QLineEdit *edt)
{
    QString fileName = QFileDialog::getOpenFileName(this, title);
    edt->setText(fileName);
}

void MainWindow::on_fromToolBtn_clicked()
{
    this->setFile("File to convert", this->ui->fromLineEdit);
}

void MainWindow::on_toToolBtn_clicked()
{
    this->setFile("File for converted image", this->ui->toLineEdit);
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
